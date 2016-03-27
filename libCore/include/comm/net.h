#ifndef NET_H
#define NET_H

#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <vector>
//#include "mylist.h"
#include "arch.h"
#include "celltype.h"
#include "bevstream.h"

#ifdef WIN32

#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#endif


struct evconnlistener;
struct event_base;
class Net;

event_base* get_default_evbase();

//网络包
struct Packet
{
	int sn_;
	Buf* buf_;
	Packet(int sn, Buf* buf)
	{
		sn_ = sn;
		buf_=buf;
	}
	int Sn()
	{
		return sn_;
	}
	Buf* getBuf()
	{	return buf_; }
};


const int FID_connectError=3;
const int FID_hearBeat=8;
const int FID_setNewSeed=9;
const int SESSION_ACCELERATE=1;
enum S_STATUS{
	SS_OK,
	SS_WAITACK,
	SS_NEWED,
	SS_CLOSED,
};


//客户端到服务端的一个连接
class Session : public BevStream
{
public:
	Session(bufferevent* bev, Net* net, const char* ip);
	~Session();

	void doMsg(Buf* buf=NULL);
	void close();
	void closeOnWrite();
	void cachePacket(int sn, Buf* buf);

	Net* net() const { return net_; }
	uint sn() const { return sn_; }
	const std::string& remoteIP() const { return remoteip_; }
	uint pingCount() const { return pingCount_; }

	void tick(uint ts);
	void checkAcc();
	int netMode() const;

	virtual void write(void* data, uint len);
	virtual void write(const Buf& buf);
	virtual void copyOnWrite(void* data, uint len);

	void status(S_STATUS ss) { status_ = ss; }
	S_STATUS status() const { return status_; }
	
	bool redirect(Session* other, int from);

private:
	static uint sessionID_;
	uint sn_;
	Net* net_;
	uint lastTick_;
	int pingCount_;
	int timecheat_;
	int timediff_;
	int clienttime_;
	S_STATUS status_;					//状态
	std::string remoteip_;				//远程地址
	std::vector<Packet> cachedPacket_;	//缓存网络包
	friend class Net;
};


#define NET_SEND_CLOSE 1
#define LM_BINARY (1<<0)
#define LM_CRYPT (1<<1)
#define LM_TEXT	(1<<2)
#define LM_CHECKOVERTIME (1<<3)
#define LM_LOOP (1<<4)

//服务器网络模块
class Net
{
public:
	Net(int mode);
	virtual ~Net();

	void init();
	int netListen(int port);
	int start();

	void update(uint dtime);
	void onConnect(Session* session);

	void closeForReason(int sn,int reason, int waitfor);
	void closeConnect(Session* session);
	void closeConnect(uint sn);
	void closeAfterSend(uint waitfor=0)
	{
		sendFlag_ = NET_SEND_CLOSE;
		closeWaitFor_ = waitfor;
	}

	void SendString(int sn, const char* str, uint size);
	void sendPacket(int sn, const Buf& buf);
	void sendPacket(int sn, int fid, const char* msg, uint msglen);
	void sendPacket(int sn, int fid, const Buf& data);
	void sendPacket(Session* session, const Buf& data);
	void sendPacketAll(Buf& buf);

	int mode() const { return mode_; }
	void enableEncrypt() { mode_|=LM_CRYPT; }
	
	bool sessionMatch(int oldsn, int newsn, int packetsn);
	int tickSession(int sn, uint timestamp);
	void setSessionSeed(int sn, int seed);
	const char* sessionHostIP(int sn)const;
	Session* getSession(int sn);

	typedef std::map<uint, Session*> SessionMap_t;

protected:
	friend void handleCommand(Net* net, Buf* buf);
	friend void* netWrapper(void* p);

	void closeAfterSend(Session* session);
	void resetCloseFlag();
	void checkOvertimeSession(uint dtime);
	int mode_;
	int sendFlag_;
	int	closeWaitFor_;
	SessionMap_t sessionMap_;
	std::vector<Session*> freeSessions_;	//关闭的连接队列
	evconnlistener* listener_;
};




#endif //NET_H


