#include <string.h>
#include <assert.h>
#include <errno.h>
#include <time.h>
#include "celltype.h"
#include <event2/thread.h>
#include "mqnet.h"
#include "buf.h"
#include "app.h"
#include "lunar.h"
#include "arch.h"
#include "log.h"

#define PT_SERVER_START 100050	//连接router
#define PT_SERVER_OFFLINE 100051//
#define PT_SERVER_STOP 100052	//


static int MaxPackHandle = 1000;

//ZMQ零拷贝释放数据回调
void freeMQMsgBuf(void *data, void *hint)
{
	free(data);
}

void freemsg(const void *data, size_t datalen, void *extra)
{
	zmq_msg_t* msg = (zmq_msg_t*)extra;
	zmq_msg_close(msg);
	delete msg;
}

//================================================================
//
MsgChannel::MsgChannel(void* socket, int len)
	:socket_(socket),
	pos_(0),
	len_(len)
{
	if (len> 655350)
	{
		ERRLOG("MsgChannel:TOO large msg %d", len);
		len = 655350;
	}
	zmq_msg_init_size(&msg_, len);
}

MsgChannel::~MsgChannel()
{
	zmq_msg_close(&msg_);
}

//加入数据
void MsgChannel::pushData(const void* data, int len)
{
	assert(pos_+len<=len_);
	char *dest = (char*)zmq_msg_data(&msg_);
	memcpy(dest+pos_, data, len);
	pos_ += len;
}

//加入数据
void MsgChannel::pushData(const Buf& value)
{
	int len = value.getLength();
	assert(pos_+len<=len_);
	char *dest = (char*)zmq_msg_data(&msg_);
	value.peek(dest+pos_, len);
	pos_ += len;
}

//发送数据
void MsgChannel::send()
{
	int rc = zmq_sendmsg(socket_, &msg_, ZMQ_NOBLOCK);
	if (rc <= 0)
	{
		CRITICAL("MsgChannel:zmq send failed, %d, %d", rc, errno);
	}
}

//================================================================

MQNet::MQNet()
{
	ctx_ = NULL;
	socket_= NULL;
	myid_ = 0;
	serverok_ = false;
	connTime_ = 0;
	flag_= MQ_INNER;
	ctx_ = zmq_init(1);
}


MQNet::~MQNet()
{
	//关闭zmq
	int zero = 0;
	if (flag_&MQ_INNER && socket_)
	{
		zmq_setsockopt(socket_, ZMQ_LINGER, &zero, sizeof(zero));
		zmq_close(socket_);
		socket_ = NULL;
	}

	if (flag_&MQ_INNER && !dbsockets_.empty())
	{
		for (uint n=0; n < dbsockets_.size(); ++n)
		{
			void *dbsocket = dbsockets_[n];
			zmq_setsockopt(dbsocket, ZMQ_LINGER, &zero, sizeof(zero));
			zmq_close(dbsocket);
			socket_ = NULL;
		}
	}

	zmq_term(ctx_);
	ctx_ =NULL;
	LOG("finalized zmq objects");
}

void MQNet::connect(int myid, const char* addr)
{
	NOTICE("Connect router server %s", addr);
	myid_ = myid;
	bool first = false;

	if (!socket_)
	{
		//给连接的对端RR地分发消息，对收到的消息公平排队
		socket_ = zmq_socket(ctx_, ZMQ_DEALER);

		//ZMQ_IDENTITY选项会设置socket的身份ID。socket的身份ID只会能在请求/回复模式中使用
		zmq_setsockopt(socket_, ZMQ_IDENTITY, &myid_, sizeof(myid_));
		first = true;
	}

	zmq_connect(socket_, addr);

	if (first)
	{
		int digi = 10001;
		sendTo(myid_, PT_SERVER_START, &digi, sizeof(digi));
		connTime_ = timer_get_time();
	}
}

void MQNet::connectDB(const char *addr)
{
	NOTICE("*************Connect db server %s", addr);
	void *sock = zmq_socket(ctx_, ZMQ_DEALER);
	zmq_setsockopt(sock, ZMQ_IDENTITY, &myid_, sizeof(myid_));

	//设置发送缓冲区大小，测试功能
	int sendBuf = 10000;
	size_t len = sizeof(sendBuf);

	//ZMQ_SNDHWM属性将会设置socket参数指定的socket对外发送的消息的高水位。
	//高水位是一个硬限制，它会限制每一个与此socket相连的在内存中排队的未处理的消息数目的最大值。0值代表着没有限制。
	zmq_getsockopt(sock, ZMQ_SNDHWM, &sendBuf, &len);
	if (sendBuf < 10000)
	{
		sendBuf = 10000;
	}
	zmq_setsockopt(sock, ZMQ_SNDHWM, &sendBuf, len);
	zmq_getsockopt(sock, ZMQ_SNDHWM, &sendBuf, &len);
	INFO("set game to db send buffer size:%d", sendBuf);

	int ret = zmq_connect(sock, addr);
	dbsockets_.push_back(sock);
}

void MQNet::disconnect()
{
	if (flag_&MQ_INNER)
	{
		sendTo(myid_, PT_SERVER_STOP);
		ERRLOG("MQNet:disconnect");
	}
}

//发送数据
//target:目标服务器ID
//fid:消息类型 脚本发送消息都为0
//data:数据
//size:数据大小
int MQNet::sendTo(int target, int fid, void* data, int size)
{
	MsgChannel ch(socket_, size+8);
	ch << target << fid << MsgData(data, size);
	INFO("MQNet::sendTo target = %d, fid = %d\n", target, fid);
	ch.send();
	return 0;
}

int MQNet::sendTo(int target, int fid, const Buf& args)
{
	MsgChannel ch(socket_, args.getLength()+8);
	ch << target << fid << args;
	ch.send();
	return 0;
}

//发送到游戏器
int MQNet::methodTo(int target, int fid, int sn, void* data, int size)
{
	MsgChannel ch(socket_, size+12);
	ch << target << fid << sn << MsgData(data, size);
	ch.send();
	return 0;
}

//发送到游戏器
int MQNet::methodTo(int target, int fid, int sn, const Buf& args)
{
	MsgChannel ch(socket_, args.getLength()+12);
	ch << target << fid << sn << args;
	ch.send();
	return 0;
}

//发送到DB服务器
int MQNet::methodToDB(int channel, int target, int fid, const Buf& args)
{
	if (channel >= (int)dbsockets_.size())
	{
		return -1;
	}
	void *sock = NULL;
	if (channel == -1)
	{
		sock = socket_;
	}
	else
	{
		sock = dbsockets_[channel];
	}
	MsgChannel ch(sock, args.getLength()+8);
	ch << target << fid << args;
	ch.send();
	return 0;
}

int MQNet::methodTo(int target, int fid, const std::vector<int>& sns, const Buf& args)
{
	MsgChannel ch(socket_, args.getLength()+8+4*sns.size());
	ch << target << fid << sns.size() << sns << args;

	ch.send();
	return 0;
}


void MQNet::doMsg(int target, int fid, Buf* buf)
{
	ServerApp::get()->doMqMsg(target, fid, buf);
}

//轮询提取消息
void MQNet::update(uint dtime)
{
	if (socket_ && (flag_&MQ_INNER) && !serverok_ && timer_get_time()-connTime_>10000)
	{
		//ERRLOG("Connect router server fail");
		connTime_ = timer_get_time();
	}

	readChannels();
}

void MQNet::useOutterSocket(void *socket)
{
	flag_ = MQ_OUTTER;
	socket_ = socket;
}

void MQNet::readFromRouter()
{
	int mp = 0;
	while(socket_ && (flag_&MQ_INNER))
	{
		//控制每次处理包数量
		if (mp++ > MaxPackHandle)
		{
			ERRLOG("Max pack break %d", MaxPackHandle);
			MaxPackHandle = MaxPackHandle + 1000;
			return;
		}
		zmq_msg_t *msg = new zmq_msg_t;
		int len = readChannel(socket_, msg);
		if (len < 8)
		{
			delete msg;
			break;
		}
		Buf buf;
		//just set prt to read,not copy
		buf.refWrite((char*)zmq_msg_data(msg), len, freemsg, msg);

		int src, fid;	//src：发来的服务器编号 fid：消息类型
		buf >> src >> fid;
		INFO("readFromRouter src = %d,fid = %d,len = %d\n", src, fid, len);
		if (src == myid_ && fid == PT_SERVER_START)
		{
			if(len < 12)
			{
				ERRLOG("Not all server runned with same rcp version len less then 12");
				delete msg;
				break;
			}

			serverok_ = true;
			int digi;
			buf >> digi;

			if (digi != 10001)
			{
				ERRLOG("Not all server runned with same rcp version, expected[%d]", digi);
				delete msg;
				break;
			}
		}
		else if (fid == PT_SERVER_OFFLINE) 
		{
			serverok_ = false;
		}
		else
		{
			if (!serverok_)
			{
				return;
			}
			doMsg(src, fid, &buf);
		}
		
	}
	MaxPackHandle -= 1000;
	if (MaxPackHandle<1000)
	{
		MaxPackHandle = 1000;
	}
}

void MQNet::readFromDB()
{
	std::vector<void*>::iterator it = dbsockets_.begin();
	for (; it < dbsockets_.end(); ++it)
	{
		void *dbsock = *it;
		while(dbsock)
		{
			zmq_msg_t *msg = new zmq_msg_t;
			int havemsg = readChannel(dbsock, msg);
			if (havemsg<0)
			{
				delete msg;
				break;
			}
			uint len = (uint)zmq_msg_size(msg);
			assert(len>4);
			Buf buf;

			buf.refWrite((char*)zmq_msg_data(msg), len, freemsg, msg);
			int src, fid;
			buf >> src >> fid;
			doMsg(src, fid, &buf);
		}
	}
}

void MQNet::readChannels()
{
	readFromRouter();
	readFromDB();
}

int MQNet::readChannel(void* socket, zmq_msg_t *msg)
{
	zmq_msg_init(msg);	//初始化一个空的ZMQ消息结构
	//指定本次操作以非阻塞模式执行。
	//如果socket上此刻没有接收到消息，zmq_recvmsg()函数会执行失败，并设施errno的值为EAGAIN
	//此API已被弃用，建议使用zmq_msg_recv(3)函数。
	//int code = zmq_recvmsg(socket, msg, ZMQ_DONTWAIT);
	//返回消息的大小。否则返回 -1，并且设置errno的值
	int code = zmq_msg_recv (msg,  socket, ZMQ_DONTWAIT);
	if (code >= 0)	
	{
		return code;
	}
	else
	{
		int err = zmq_errno();
		if(err != EAGAIN)//no data to read
		{
			ERRLOG("MQRecv error (%d),%s", err, zmq_strerror(err));
		}
		zmq_msg_close(msg);
		return code;
	}
}

void MQNet::setcontext(void *ctx)
{
	ctx_ = ctx;
}

void MQNet::setsocket(void *socket)
{
	socket_ = socket;
}

