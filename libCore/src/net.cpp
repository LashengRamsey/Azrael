#include <errno.h>
#include <timer.h>
//#include "celltype.h"
#include <event2/thread.h>
#include <event2/listener.h>
#include "app.h"
#include "luasvr.h"
#include "timer.h"
#include "net.h"
#include "log.h"

#ifdef WIN32
#include <winsock2.h>
#else
#include <netinet/tcp.h>
#include "bevstream.h"
#endif//!WIN32


static unsigned char header = PACKET_HEAD;
static unsigned char ender = PACKET_END;


#define CC_CLIENT_ACCELERATE 6

uint Session::sessionID_ = 0;

static bool CheckOverTime = false;
static uint OverTime = 30000;
static int AccCheck = 2000;
static int TimeCheatCount = 3;
static const uint MaxCachedPackt = 256;

static int keepAlive = 1;
static int keepIdle = 30;
static int keepInterval = 5;
static int keepCount = 3;
static int tmpFid = 0;

//ȫ�ֵ�event_base
static event_base* evbase_ = NULL;
event_base* get_default_evbase()
{
	if (!evbase_)
	{
		evbase_ = event_base_new();
	}
	return evbase_;
}

//�ɶ��ص�
static void read_cb(struct bufferevent* bev, void* ctx)
{
	Session* session = (Session*)ctx;
	if(session->netMode() & LM_TEXT)	
	{
		session->decodeText();
	}
	else
	{
		session->decode();
	}
}

//�¼��ص�
static void event_cb(struct bufferevent* bev, short events, void* ctx)
{
	Session* session = (Session*)ctx;

	int fd = bufferevent_getfd(bev);

	if(session && events & BEV_EVENT_EOF)
	{
		INFO("Connection closed fd:%d,session:%d by %s", fd, session->sn());
		session->close();
	}
	else if(session && events & BEV_EVENT_ERROR)
	{
		int code = EVUTIL_SOCKET_ERROR();
		ERROR("Event %d, fd %d, error %d(%s) and disconnected client %d:%s",
			code,evutil_socket_error_to_string(code),session->sn(),
			session->remoteIP().c_str());
		session->close();
	}
	else if(!session)
	{
		ERROR("Session missed");
	}
}

//��������
static void accept_conn_cb(struct evconnlistener* listener, 
						   evutil_socket_t fd, struct sockaddr* address,
						   int socklen, void* ctx)
{
	Net* net = (Net*) ctx;
	sockaddr_in* sin = (sockaddr_in*)address;
	const char* hostip = inet_ntoa(sin->sin_addr);

	if(hostip)
	{
		//����Ϊ������
		evutil_make_socket_nonblocking(fd);
		//���ؼ�����������event_base
		event_base* base = evconnlistener_get_base(listener);

		//ʹ������bufferevent,���Զ��շ�����
		//���������׽��ֵ�bufferevent
		bufferevent* bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
		
		Session* session = new Session(bev, net, hostip);
		bufferevent_setcb(bev, read_cb, NULL, event_cb, session);	//���ö�д�ص�
		bufferevent_enable(bev, EV_READ|EV_WRITE);					//����bev���ĵ��¼�

		int tcp_nodelay = 1;
		//����tcpΪ���ӳٷ���
		if (setsockopt(bufferevent_getfd(bev), IPPROTO_TCP, TCP_NODELAY,
			(const char*)&tcp_nodelay, sizeof(int)) == -1)
		{
			//LOG("RemoteSessionConnected error,set TCPNODELAY?\n");
		}
		else
		{
			//LOG("addr=%s,setTcpNodelay ok\n", session->remoteIP().c_str());
		}
		INFO("New connection from %s,fd %d,sn %d", hostip, fd, session->sn());
		net->onConnect(session);
	}
	else
	{
		ERROR("Can't get remoteip");
	}

}

//sessionд������ص�
static void close_write_cb(struct bufferevent* bev, void* ctx)
{
	struct evbuffer *output = bufferevent_get_output(bev);
	size_t size = evbuffer_get_length(output);

	if (size == 0)
	{
		Session *session = (Session*)ctx;
		session->close();
	}
}

//libevent log��������
static void write_to_file_cb(int serverity, const char* msg)
{
	const char *s;
	switch(serverity)
	{
	case _EVENT_LOG_DEBUG:
		s = "debug";
		break;
	case _EVENT_LOG_MSG:
		s = "msg";
		break;
	case _EVENT_LOG_WARN:
		s = "warn";
		break;
	case _EVENT_LOG_ERR:
		s = "err";
		break;
	default:
		s = "?";
		break;
	}
	LOG("event [%s] %s\n", s, msg);
}

//libevent fatal����
static void write_to_fatal_cb(int err)
{
	LOG("event exit error,errorId:%d\n", err);
}

//listener error callback
static void accept_error_cb(struct evconnlistener *listener, void *ctx)
{
	struct event_base *base = evconnlistener_get_base(listener);
	int err = EVUTIL_SOCKET_ERROR();
	ERROR("Got an error %d(%s) on the listener Shutting down.\n", err, evutil_socket_error_to_string(err));

	//event_base_loopexit������event_base�ڸ���ʱ��֮��ֹͣѭ�������tv����ΪNULL��event_base������ֹͣѭ����û����ʱ��
	//���event_base��ǰ����ִ���κμ����¼��Ļص�����ص���������У�ֱ�����������м����¼��Ļص�֮���˳���
	event_base_loopexit(base, NULL);
}


Session::Session(bufferevent* bev, Net* net, const char* ip)
	:BevStream(bev),
	sn_(++sessionID_),
	net_(net),
	lastTick_(0),
	pingCount_(1),
	timecheat_(0),
	timediff_(0),
	clienttime_(0),
	status_(SS_NEWED),
	remoteip_(ip)
{
}

Session::~Session()
{
	//LOG("Session be free %d", sn_);
	if (bev_)
	{
		bufferevent_free(bev_);
		bev_=NULL;
	}
}

void Session::close()
{
	net_->closeConnect(this);
}

void Session::cachePacket(int sn, Buf *buf)
{

}

void Session::tick(uint ts)
{
	lastTick_ = ts;
	pingCount_++;
	checkAcc();
}



void Session::checkAcc()
{
	if (AccCheck == 0 || lastTick_ == 0)
	{
		return;
	}
	uint timestamp = timer_get_time();
	int delta = (lastTick_ - timestamp)-timediff_;
	timediff_ = (lastTick_ - timestamp);

	if (delta > AccCheck)
	{
		timecheat_++;
	}
	else
	{
		timecheat_--;
		if (timecheat_ < 0)
			timecheat_ = 0;
	}


	if (timecheat_>= TimeCheatCount)
	{
		ERROR("Close %s,due to accelerate", remoteip_.c_str());
		net_->closeForReason(sn_, CC_CLIENT_ACCELERATE, 5000);
	}
}

int Session::netMode() const
{
	ASSERT(net_);
	return net_->mode();
}


//д������
void Session::write(void* data, uint len)
{
	if (status_ == SS_WAITACK)
		return;
	BevStream::write(data, len);
}

void Session::write(const Buf& buf)
{
	if (status_ == SS_WAITACK)
		return;
	BevStream::write(buf);
}

void Session::copyOnWrite(void* data, uint len)
{
	if (status_ == SS_WAITACK)
		return;
	BevStream::copyOnWrite(data, len);
}

//�������ת�Ƶ�other����
bool Session::redirect(Session* other, int from)
{
	if(cachedPacket_.empty())
		return false;

	std::vector<Packet>::iterator it = cachedPacket_.begin();
	if(it->Sn() > from)
		return false;

	it += (from - it->Sn() + 1);

	for (; it < cachedPacket_.end(); ++it)
	{
		Buf *buf = it->getBuf();
		other->newpacket(buf->getLength());
		other->write(*buf);
		other->cachePacket(it->Sn(), buf);
		other->packetWriteSN_ = it->Sn();
	}
	cachedPacket_.clear();
	return true;
}


void Session::doMsg(Buf* buf)
{
	ServerApp::get()->doNetMsg(sn_, buf);
}



void Session::closeOnWrite()
{
	//���ò����ص�
	//readcb,writecb,eventcb
	bufferevent_setcb(bev_, NULL, close_write_cb, NULL, this);
	bufferevent_enable(bev_, EV_WRITE);
}



void closeNow(Session* session)
{
	session->closeOnWrite();
}

void closeWait(void *userdata)
{
	Net* net = ServerApp::get()->getNet();
	int sn = (long)userdata;
	net->closeConnect(sn);
	return ;
}


Net::Net(int mode)
	:mode_(mode),
	sendFlag_(0),
	closeWaitFor_(0)
{
	//log��������
	event_set_log_callback(write_to_file_cb);
	event_set_fatal_callback(write_to_fatal_cb);
	//event_enable_debug_mode();	//����ģʽ
	evbase_ = get_default_evbase();
	listener_ = NULL;
}

Net::~Net()
{
	if (listener_)
	{
		//�ͷ����Ӽ�����
		evconnlistener_free(listener_);
	}
	event_base_loopbreak(evbase_);
	event_base_free(evbase_);
	evbase_ = NULL;

}


void Net::init()
{
	//�����˿�
	int port = Config::GetIntValue("BindPort");
	if ( port == 0 )
	{
		ERROR("Network Net::init port is 0");
		port = 30000;
	}

	//�Ƿ����
	/*int crypt = Config::GetIntValue("Crypt");
	if (crypt)
	{
		enableEncrypt();
	}*/
	netListen(port);
}

//���ü���
int Net::netListen(int port)
{
	if (listener_)
	{
		ERROR("Network had started listened");
		return 0;
	}

	struct sockaddr_in listen_addr;
	memset(&listen_addr, 0, sizeof(listen_addr));
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = INADDR_ANY;
	listen_addr.sin_port = htons(port);
	//���ü���
	listener_ = evconnlistener_new_bind(evbase_, accept_conn_cb, this,
		LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE, 4096,
		(struct sockaddr*)&listen_addr, sizeof(listen_addr));
	if (!listener_)
	{
		ERROR("Could't create listener, maybe process alread started");
		return 1;
	}
	evconnlistener_set_error_cb(listener_, accept_error_cb);
	NOTICE("Net start at port %d\n", port);
	return 0;
}

int Net::start()
{
	return 1;
}

//update
void Net::update(uint dtime)
{
	//��鳬ʱ��session
	if ((mode_ & LM_CHECKOVERTIME) && CheckOverTime)
		checkOvertimeSession(dtime);

	//�ͷŹرյ�session
	for (std::vector<Session*>::iterator it = freeSessions_.begin();
		it != freeSessions_.end(); ++it)
	{
		delete (*it);
	}
	freeSessions_.clear();
}	

//��鳬ʱ
void Net::checkOvertimeSession(uint dtime)
{
	static uint checktime = 0;
	checktime += dtime;

	if (checktime < OverTime)
		return;
	checktime = 0;

	std::vector<Session*> sess;
	//{
	SessionMap_t::iterator it = sessionMap_.begin();
	for(;it != sessionMap_.end(); ++it)
	{
		Session* session = it->second;
		if (session)
		{
			uint lastts = session->lastTick_;
			if (lastts != 0 && session->pingCount_ == 0)
			{
				sess.push_back(it->second);
			}
			session->pingCount_ = 0;
		}
	}
	//}

	//{
	std::vector<Session*>::iterator st = sess.begin();
	for (; st!= sess.end(); ++st)
	{
		ERROR("Close connection %d,%s, due to overtime", 
			(*st)->sn(),(*st)->remoteIP().c_str());
		this->closeConnect(*st);
	}
	//}
}

void Net::onConnect(Session* session)
{
	SessionMap_t::iterator it = sessionMap_.lower_bound(session->sn());
	if(it != sessionMap_.end() && it->second->sn() == session->sn())
	{
		ERROR("BevMap had value with same key %d", session->sn());
		return;
	}
	//֪ͨ�ű�����������
	sessionMap_.insert(it, SessionMap_t::value_type(session->sn(), session));
	LuaSvr::call("CHandlerConnect", "i", session->sn());
	session->status(SS_OK);
}

//�ر�����
void Net::closeConnect(uint sn)
{
	if(sn)
	{
		SessionMap_t::iterator it = sessionMap_.find(sn);
		if (it != sessionMap_.end())
		{
			Session* session = it->second;
			closeConnect( session );
		}
	}
}

void Net::closeConnect(Session *session)
{
	if (session->status() != SS_CLOSED)
	{
		session->status(SS_CLOSED);
		SessionMap_t::iterator it = sessionMap_.find(session->sn());
		if ( it != sessionMap_.end())
		{
			ServerApp::get()->doNetDisconnect(session->sn());
			LuaSvr::call("CHandlerDisconnect", "i", session->sn());
			freeSessions_.push_back(session);
			sessionMap_.erase(it);
		}
		else
		{
			ERROR("double free session %d", session->sn());
		}
	}
}

void Net::closeForReason(int sn,int reason, int waitfor)
{
	closeAfterSend(waitfor);
	ERROR("CloseConnection:%d,reason:%d", sn, reason);
}



void Net::closeAfterSend(Session* session)
{
	if (sendFlag_ & NET_SEND_CLOSE)
	{
		if(closeWaitFor_ == 0)
			closeNow(session);
	}
}

void Net::resetCloseFlag()
{
	sendFlag_ = 0;
	closeWaitFor_ = 0;
}

void Net::SendString(int sn, const char* str, uint size)
{
	if ( sn )
	{
		SessionMap_t::iterator it = sessionMap_.find(sn);
		if (it != sessionMap_.end())
		{
			Session *session = it->second;
			session->write((void*)str, size);
			closeAfterSend(session);
		}
		else
		{
			ERROR("Some sn can't find %d, so some packet can't be send", sn);
		}
	}
}

void Net::sendPacket(int sn, const Buf& buf)
{
	Session *session = getSession(sn);
	if (session && session->sn() != (uint)sn)
	{
		ERROR("Session wrong %d", sn);
		return ;
	}
	sendPacket(session, buf);
}

void Net::sendPacket(int sn, int fid, const char* msg, uint msglen)
{
	Buf bufout;
	bufout << (int8)PACKET_HEAD
		<< msglen + 1
		<< fid
		<< BufData(msg, msglen)
		<< (int8)PACKET_END;

	this->sendPacket(sn, bufout);
}

void Net::sendPacket(int sn, int fid, const Buf& data)
{
	Buf bufout;
	int len = 0;
	if (fid < 256)
	{
		len = conv_num(data.getLength() + 1);
		bufout << (int8)PACKET_HEAD
			<< len
			<< (uint8)fid
			<< data
			<< (int8)PACKET_END;
	}
	else
	{
		len = conv_num(data.getLength() + 2);
		bufout << (int8)PACKET_HEAD
			<< len
			<< (uint16)fid
			<< data
			<< (int8)PACKET_END;
	}
	this->sendPacket(sn, bufout);
}

void Net::sendPacket(Session* session, const Buf& buf)
{
	if (session)
	{
		uint size = buf.getLength();
		session->write(buf);
		closeAfterSend(session);
	}
	resetCloseFlag();
}

void Net::sendPacketAll(Buf& buf)
{
	uint size = buf.getLength();
	LocalBuf data(size);
	buf.read(&data, size);
	SessionMap_t::iterator it = sessionMap_.begin();
	for(;it!=sessionMap_.end();it++)
	{
		Session *session = it->second;
		session->newpacket(size);
		session->copyOnWrite(data, size);
		closeAfterSend(session);
	}
	resetCloseFlag();
}




//
bool Net::sessionMatch(int oldsn, int newsn, int packetsn)
{
	SessionMap_t::iterator oldit = sessionMap_.find(oldsn);
	SessionMap_t::iterator newit = sessionMap_.find(newsn);

	if (oldit != sessionMap_.end() && newit != sessionMap_.end())
	{
		bool ret = oldit->second->remoteIP() == newit->second->remoteIP();
		if (ret)
		{
			ret &= oldit->second->redirect(newit->second, packetsn);
		}
		return ret;
	}
	return false;
}
int Net::tickSession(int sn, uint timestamp)
{
	SessionMap_t::iterator it = sessionMap_.find(sn);
	if (it != sessionMap_.end())
	{
		it->second->tick(timestamp);
	}
	return 0;
}

void Net::setSessionSeed(int sn, int seed)
{
	SessionMap_t::iterator it = sessionMap_.find(sn);
	if (it != sessionMap_.end())
	{
		it->second->setSeed(seed);
	}
}

//����id��ȡsessionԶ��ip
static std::string HostClosed = "0.0.0.0";
const char* Net::sessionHostIP(int sid) const
{
	SessionMap_t::const_iterator it = sessionMap_.find(sid);
	if (it != sessionMap_.end())
	{
		it->second->remoteIP().c_str();
	}
	return HostClosed.c_str();
}

//����id��ȡsession
Session* Net::getSession(int sid)
{
	SessionMap_t::iterator it = sessionMap_.find(sid);
	if (it != sessionMap_.end())
	{
		it->second;
	}
	return NULL;
}







