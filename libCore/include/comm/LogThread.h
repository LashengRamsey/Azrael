#ifndef _LOGTHREAD_H
#define _LOGTHREAD_H
#include "pthread.h"
#include "queue"
#include "map"
#include "string"
#include "stdio.h"
#include "string.h"
#include "bufferstring.h"

typedef enum
{
	LOG_STDOUT,
	LOG_INFO,
	LOG_WAR,
	LOG_ERROR,
}LogType;

typedef enum
{
	LOG_LOCAL_NONE	,
	LOG_WRITE_DIRECT,		
	LOG_WRITE_BUFFER,		
}logWriteType;

typedef enum
{
	LOG_FILE_C	,
	LOG_FILE_LINUX,			//cÎÄ¼þ
}LogfileType;

#ifdef LOG2FILE_WHTH_C
#define  LOGFILE_fd FILE*
#else
#define  LOGFILE_fd int
#endif

typedef struct LOGFILE
{
	LOGFILE_fd fd;
	LogfileType type;
	struct bufferevent* event;
	unsigned int createHour;
	unsigned int createMonth;
	unsigned int createDay;
	unsigned int createYear;
};

class LogThread
{
public:
	typedef struct
	{
		std::string iFile;
		std::string iLogEvent;
		LogType iType;
		bool format;
	}log_t;
	virtual ~LogThread();
	static LogThread* CreateLogThread();
	bool stop();
	void loop();
	void addLog(LogType type,const char* logEvent,const char* file = 0,bool format = true);
protected:
	LogThread();
	bool setupThread();
	log_t* popLog();
	void writestring(log_t* log);
private:
	pthread_t iThreadId;
	bool iKilled;
	bool iRun;
	pthread_mutex_t iFrontLogQueueLock;
	std::queue<log_t*> iFrontLogQueue;

	pthread_mutex_t iBackLogQueueLock;
	std::queue<log_t*> iBackFrontLogQueue;

	typedef std::map<std::string,LOGFILE*> CachedFileMap;
	CachedFileMap iCachedFile;
};
#endif
