#ifndef DBSERVER_H
#define DBSERVER_H

#include <stdio.h>
#include "app.h"
#include "mqnet.h"
#include "buf.h"
#include "arch.h"
#include "net.h"
#include "log.h"
#include "luasvr.h"
#include "luaMysql.h"


class DBServer : public ServerApp
{
public:
	DBServer();
	virtual ~DBServer();

	void run();
	void onInited();
	void onUpdate(unsigned int dtime);
	int SendToGameServer(int target, int fid, int sn, const Buf& buf);

protected:
	void createContext();
	void readMqMsg();
	void RegisterFunc();

protected:
	void *socket_;
};


#endif //!DBSERVER_H

