#include <stdio.h>
#include "DBServer.h"
#include "pthread.h"
#include "arch.h"



/**线程标识*/
pthread_t mThreadId;
DBServer* server;

void *threadProc(void *arg) {
	//DEBUG_TRY;
	DBServer *me = (DBServer*)arg;
	if(me)
	{
		me->start();
	}	
	//DEBUG_CATCH_DUMP(LOG_LOGIC_THREAD);
	return NULL;
}

bool startThread(int argc, char *argv[])
{
	DEBUG_TRY;
	server = new DBServer();
	if(server->init(argc, argv))
	{
#ifdef WIN32
		//启动对应线程开启事件循环
		pthread_attr_t  attr;
		int             ret;
		pthread_attr_init(&attr);
		if ((ret = pthread_create(&mThreadId, &attr, threadProc, server)) != 0) 
		{
			printf("Can't create thread: %s\n",strerror(ret));
		}
#else
		server->start();
#endif
		return true;
	}
	return false;
	DEBUG_CATCH;
}


int main(int argc, char *argv[])
{
#ifdef WIN32
#	ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);
#	endif
#endif
	///DEBUG_TRY;
	Config::SetConfigName(argv[1]);
	StartLogServer();

	if(!startThread(argc, argv))
	{
		//LogDirect("startThread error.", LOG_GAMESERVER_INIT);
	}

	//DEBUG_CATCH_DUMP(LOG_MAIN_THREAD);

	char input[200];	
	while(1)
	{	
		fgets(input,200,stdin);
		break;	
	}

	//while(!server->threadKilled()){}
	delete server;
	server = NULL;
	ExitLogServer();
	return 0;
}