#ifndef WIN32
#include <netinet/tcp.h>
#include <netdb.h>
#endif

#include "connection.h"
#include "log.h"
#include "app.h"
#include "luaglobal.h"
#include "luaNetwork.h"
#include "LuaSvr.h"
#include "timer.h"

#define CONN_ERR_ID 1
#define CONN_ERR_CONN_TIMEOUT 2

static unsigned char header = PACKET_HEAD;
static unsigned char ender = PACKET_END;


LUA_IMPLE(Connection, Connection);
LUA_METD(Connection)
L_METHOD(Connection, c_Connect)
L_METHOD(Connection, c_Write)
L_METHOD(Connection, c_Close)
L_METHOD(Connection, c_IsConnect)
L_METHOD(Connection, c_RawWrirte)
LUA_METD_END
LUA_FUNC(Connection)
LUA_FUNC_END



void Connection::onRead()
{
	if (raw_)
	{
		decodeText();
	}
	else
	{
		decode();
	}
}

void Connection::read_cb(struct bufferevent* bev, void* ctx)
{
	Connection *conn = (Connection*)ctx;
	conn->onRead();
}


void Connection::event_cb(struct bufferevent* bev, short events, void* ctx)
{
	Connection *conn = (Connection*)ctx;
	if (conn && events&BEV_EVENT_EOF)
	{
		conn->close();
	}
	else if (conn && events&BEV_EVENT_ERROR)
	{
		conn->onError(CONN_ERR_ID);
	}
	else if (conn && events&BEV_EVENT_CONNECTED)
	{
		conn->onConnected();
	}
}
int Connection::timeout_cb(void* ud)
{
	Connection *conn = (Connection*)ud;
	conn->onConnectTimeOut();
	return 0;
}

Connection::Connection()
{
	connected_ = false;
	bev_ = NULL;
	coutTimer_ = 0;
	sn_ = 0;
	raw_ = false;
	data_  = NULL;
	ref_ = LUA_NOREF;
	seed_ = 0;
}

Connection::~Connection()
{
	close();
	LOG("LuaConnection be freed %s", userData_.c_str());
}

int Connection::c_IsConnect( lua_State* L)
{
	lua_pushboolean(L, isConnected());
	return 1;
}

int Connection::connect(const char* host, int port, int itmeout)
{
	if (bev_)
		return 1;

	hostent *h = gethostbyname(host);
	if (!h)
	{
		ERROR("Could't lookup %s:%d", host, h_errno);
		return 1;
	}
	if (h->h_addrtype != AF_INET)
	{
		ERROR("No ipv6 support, sorry.");
		return 1;
	}

	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd<0)
	{
		ERROR("Create new socket failed");
		return 1;
	}

	evutil_make_socket_nonblocking(fd);
	bev_ = bufferevent_socket_new(get_default_evbase(), fd, BEV_OPT_CLOSE_ON_FREE);
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr = *(struct in_addr*)h->h_addr;

	int r = bufferevent_socket_connect(bev_, (struct sockaddr*)&sin, sizeof(sin));
	bufferevent_setcb(bev_, Connection::read_cb, NULL, Connection::event_cb, this);
	bufferevent_enable(bev_, EV_READ|EV_WRITE);

	return r;
}

bool Connection::isConnected()
{
	return (connected_&&bev_);
}

void Connection::close()
{
	if(bev_)
	{
		bufferevent_free(bev_);
		bev_ = NULL;
	}
	removeTimer();
	connected_ = false;
	LOG("connect closed, error:%s", evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
	onLuaClose();

}

void Connection::onError(int code)
{
	LOG("Connection occur error code:%d %s", code, userData_.c_str());
	close();
}

void Connection::onConnected()
{
	if (connected_)
	{
		return;
	}

	int fd = bufferevent_getfd(bev_);
	int tcp_nodelay = 1;
	if (setsockopt(bufferevent_getfd(bev_), IPPROTO_TCP, TCP_NODELAY,
		(const char*)&tcp_nodelay, sizeof(int)) == -1)
	{
		LOG("RemoteSessionConnected eror, set TCPNODELAY?");
	}

	int keepalive = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (const char*)&keepalive, sizeof(keepalive)) == -1 )
	{
		LOG("set keep_alive eror");
	}

	connected_ = true;
	removeTimer();
	//ServerApp::get()->doNetConnect(this);
	onLuaConnect();
}

void Connection::onConnectTimeOut()
{
	if (coutTimer_ > 0)
	{
		LOG("onConnectTimeOut %s", userData_.c_str());
		onError(CONN_ERR_CONN_TIMEOUT);
	}
}


void Connection::removeTimer()
{
	if (coutTimer_)
	{
		//Timer::deleteTimer(coutTimer_);
		coutTimer_ = 0;
	}
}

void Connection::doMsg(Buf* buf)
{
	ServerApp::get()->doNetMsg(sn_, buf);
	onLuaMsg(buf);
}

void Connection::sendTo(int fid, int64 eid, Buf& buf)
{
	if (!isConnected())
		return;
	int len =conv_num(buf.getLength()+10);
	Buf bufout;
	bufout << header << len << conv_num(eid) << conv_num((int16)fid) << buf << ender;
	write(bufout);
}

void Connection::sendTo(int fid, int64 eid, int key, Buf& buf)
{
	if (!isConnected())
		return ;
	int len = conv_num(buf.getLength() + 14);
	Buf bufout;
	bufout << header << len << conv_num(eid) << conv_num((uint16)fid) 
		<< conv_num(eid) << buf << ender;
}

int Connection::c_Write( lua_State* L)
{
	if(!bev_)
	{
		luaL_error(L, "Connection hadn't created");
		return 0;
	}
	int fid, t;
	int64 uid;
	Lua::argParse(L, "ilt", &fid, &uid, &t);

	Buf buf;
	int len = lua_objlen(L, t);
	for(int i=1; i <= len; ++i)
	{
		lua_rawgeti(L, t, i);
		if (!lua_istable(L, -1))
		{
			luaL_error(L, "except table at index:%d", i);
		}
		LuaNetwork::resolvePacketTableItem(L, &buf);
		lua_pop(L, 1);
	}
	sendTo(fid, uid, buf);
	return 0;
}

int Connection::c_RawWrirte( lua_State* L)
{
	if(!bev_)
	{
		luaL_error(L, "Connection hadn't created");
		return 0;
	}

	size_t len = 0;
	const char* text = luaL_checklstring(L, -1, &len);
	if (text)
	{
		write((void*)text, len);
	}
	return 0;
}

int Connection::c_Close( lua_State* L)
{
	close();
	return 0;
}

int Connection::c_Connect( lua_State* L)
{
	char *ip;
	int port, notify, timeout = 30;
	Lua::argParse(L, "sit|ib", &ip, &port, &notify, &timeout, &raw_);
	//lua_pushvalue(L,n)是取得原来栈中的第notify个元素，压到栈顶
	//把堆栈上给定有效处索引处的元素作一个拷贝压栈
	lua_pushvalue(L, notify);
	//int i = lua_type(L, -1);
	ref_ = luaL_ref(L, LUA_REGISTRYINDEX);
	int ret = connect(ip, port, timeout);
	if (ret != 0)
	{
		luaL_error(L, "connection server faild");
		return 0;
	}
	lua_pushinteger(L, ret);
	return 1;
}

void Connection::onLuaConnect()
{
	lua_State *L = LuaSvr::get()->L();
	if (!L || ref_ == LUA_NOREF)
		return;

	int top = lua_gettop(L);
	lua_getref(L, ref_);
	int i = lua_type(L, -1);
	//把 t[k] 值压入堆栈，这里的 t 是指有效索引 index 指向的值
	lua_getfield(L, -1, "onLuaConnect");
	//lua_getfield(L, -1, "xx");
	//int i2 = lua_type(L, -1);
	if (!lua_isfunction(L, -1))
	{
		lua_pop(L, 2);
		printf("Connection onLuaConnect error: not onConnect function");
		return;
	}

	Lua::Lunar<Connection>::push(L, this, false);
	LuaSvr::scriptCall(L, 1, 0);
	lua_settop(L, top);
}

void Connection::onLuaClose()
{
	lua_State *L = LuaSvr::get()->L();
	if (!L || ref_ == LUA_NOREF)
		return;

	int top = lua_gettop(L);
	lua_getref(L, ref_);
	lua_getfield(L, -1, "onLuaClose");
	if (!lua_isfunction(L, -1))
	{
		lua_pop(L, 2);
		return;
	}

	Lua::Lunar<Connection>::push(L, this, false);
	LuaSvr::scriptCall(L, 1, 0);

	lua_unref(L, ref_);
	ref_ = LUA_NOREF;
	lua_settop(L, top);
}

void Connection::onLuaMsg(Buf* buf)
{
	lua_State *L = LuaSvr::get()->L();
	if (!L || ref_ == LUA_NOREF)
		return;

	int top = lua_gettop(L);
	lua_getref(L, ref_);
	lua_getfield(L, -1, "onLuaMsg");
	if (!lua_isfunction(L, -1))
	{
		lua_pop(L, 2);
		return;
	}

	uint16 fid = buf->readShort();
	std::string data;
	buf->readText(data);

	Lua::Lunar<Connection>::push(L, this, false);
	LuaSvr::call("|iSii", fid, &data, 0, data.size());
	lua_settop(L, top);
}

void Connection::onLuaRawMsg(Buf* buf)
{
	lua_State *L = LuaSvr::get()->L();
	if (!L || ref_ == LUA_NOREF)
		return;

	int top = lua_gettop(L);
	lua_getref(L, ref_);
	lua_getfield(L, -1, "onLuaRawMsg");
	if (!lua_isfunction(L, -1))
	{
		lua_pop(L, 2);
		return;
	}

	uint16 fid = buf->readShort();
	std::string data;
	buf->readText(data);

	Lua::Lunar<Connection>::push(L, this, false);
	LuaSvr::call("|iSii", fid, &data, 0, data.size());
	lua_settop(L, top);
}
