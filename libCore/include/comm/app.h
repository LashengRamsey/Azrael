#ifndef APP_H
#define APP_H


#include <string>
#include <map>
#include "celltype.h"

class MQNet;
class Buf;

class ServerApp
{
public:
	ServerApp();
	virtual ~ServerApp();

	virtual bool init(int argc, char* argv[]);
	virtual void onInited() {}
	virtual void createContext();

	virtual void fini();
	virtual void run();
	virtual void start();
	virtual void stop();

	void update();
	virtual void onUpdate(unsigned int dtime){}

	virtual void doMqMsg(int target, int fid, Buf* buf);
	virtual void doNetMsg(int sn, Buf* buf=NULL);
	virtual void doNetConnect(class Connection* conn){}
	virtual void doNetDisconnect(int sn){}
	virtual void loop();
	virtual void connect();
	void connectRouter();
	void connectDB();

	virtual int SendPacket(int target, int fid, int sn, const Buf& buf);
	virtual int SendToGameServer(int target, int fid, int sn, const Buf& buf)
	{
		return 0;
	}


	unsigned int getServerID();
	virtual MQNet* getMQNet() { return mqnet_; }
	class Net* getNet() { return net_; }
	int isResume() const { return appResume_; }
	int loadConfig( const char* file );
	static ServerApp* get();

protected:
	volatile int appResume_;
	class MQNet* mqnet_;
	class Net* net_;
	class LuaSvr* lua_;
	class Timer* timer_;
	static ServerApp* Self_;

	const char* myName_;
	const char* sockPath_;
	unsigned int lastTickTime_;
	unsigned int dTime_;
	unsigned int memFreeTick_;

	void setupSignal();
	void rlimit();

};




#endif //APP_H

