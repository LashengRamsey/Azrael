#ifndef CONNECTION_H
#define CONNECTION_H

#include "net.h"
#include "bevstream.h"
#include "lunar.h"

#ifndef WIN32
#include <errno.h>
#endif

//用于连接到其它地址
class Connection : public BevStream
{
public:
	LUA_EXPORT(Connection)
	int c_Connect( lua_State* L);
	int c_Write( lua_State* L);
	int c_Close( lua_State* L);
	int c_IsConnect( lua_State* L);
	int c_RawWrirte( lua_State* L);

protected:
	void onLuaConnect();
	void onLuaClose();
	void onLuaMsg(Buf* buf);
	void onLuaRawMsg(Buf* buf);

public:
	Connection();
	~Connection();

	int connect(const char* host, int port, int itmeout=30);
	bool isConnected();
	void close();

	void sendTo(int fid, int64 eid, Buf& buf);
	void sendTo(int fid, int64 uid, int key, Buf& buf);
	virtual void doMsg(Buf* buf=NULL);
	void setData(void* data) {data_ = data; }
	void* getData() {return data_; }
	void setSn(int sn) { sn_ = sn; }

private:
	bool connected_;
	bool raw_;
	int coutTimer_;
	void* data_;
	int sn_;
	int ref_;
	int seed_;

	static void read_cb(struct bufferevent* bev, void* ctx);
	static void event_cb(struct bufferevent* bev, short events, void* ctx);
	static int timeout_cb(void* ud);
	
	void onRead();
	void onError(int code);
	void onConnected();
	void onConnectTimeOut();

	void removeTimer();

};

#endif //CONNECTION_H

