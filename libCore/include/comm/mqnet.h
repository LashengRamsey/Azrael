#ifndef MQNET_H
#define MQNET_H


#include <vector>
#include "zmq.h"
#include "zmq_utils.h"


#define CHANNEL_ROUTER 0
#define CHANNEL_DB 1
#define MQ_INNER 1
#define MQ_OUTTER 2


class Buf;

struct MsgData
{
	MsgData(const void* pData, uint len)
	{
		pData_ = pData;
		len_ = len;
	}

	const void* pData_;
	uint len_;
};

//封装zmq_msg_t
class MsgChannel
{
public:
	MsgChannel(void* socket, int len);
	~MsgChannel();

	template<typename T>
	MsgChannel& operator<<(const T& value)
	{
		pushData(&value, sizeof(value));
		return *this;
	}
	template<typename T>
	MsgChannel& operator<<(const std::vector<T>& value)
	{
		pushData(&value.front(), value.size()*sizeof(T));
		return *this;
	}
	MsgChannel& operator<<(const MsgData& value)
	{
		pushData(value.pData_, value.len_);
		return *this;
	}
	MsgChannel& operator<<(const Buf& value)
	{
		pushData(value);
		return *this;
	}

	void pushData(const void* data, int len);
	void pushData(const Buf& value);
	void send();

private:
	zmq_msg_t msg_;
	void* socket_;
	int pos_;
	int len_;
};


class MQNet
{
public:
	MQNet();
	~MQNet();

	virtual void connect(int myid, const char* addr);
	void connectDB(const char* addr);
	virtual void disconnect();

	//发送数据
	//target:目标服务器ID
	//fid:消息类型
	//data:数据
	//size:数据大小
	virtual int sendTo(int target, int fid, const Buf& args);
	virtual int sendTo(int target, int fid, void* data=NULL, int size=0);
	virtual int methodTo(int target, int fid, int sn, void* data=NULL, int size=0);
	virtual int methodTo(int target, int fid, int sn, const Buf& args);
	virtual int methodToDB(int channel, int target, int fid, const Buf& args);
	virtual int methodTo(int target, int fid, const std::vector<int>& sns, const Buf& args);

	virtual void doMsg(int target, int fid, Buf* buf);

	void update(uint dtime);
	void useOutterSocket(void* socket);
	void* context() const {  return ctx_; }
	void setcontext(void* ctx);
	void setsocket(void* socket);

protected:
	int readChannel(void* sockedt, zmq_msg_t* msg);
	void readChannels();
	void readFromRouter();
	void readFromDB();
	
	void* ctx_;
	void* socket_;
	std::vector<void*> dbsockets_;
	int myid_;
	bool serverok_;
	uint connTime_;
	int flag_;
	




};



#endif //MQNET_H
