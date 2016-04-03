#include "LogThread.h"
#include "arch.h"
#include <time.h>
#include "log.h"



//char logRootCWd[MAX_LOG_ROOT_CWD_SIZE]={0};
//const char* getLogRootCwd()
//{
//	DEBUG_TRY;
//	if( strlen(logRootCWd) == 0 )
//	{
//		getcwd(logRootCWd, MAX_LOG_ROOT_CWD_SIZE);
//		strncat(logRootCWd,"/log", MAX_LOG_ROOT_CWD_SIZE - strlen(logRootCWd));
//	}
//	return logRootCWd;
//	DEBUG_CATCH;
//}

typedef struct LogServerInfo
{
	unsigned int noExecSize;
	unsigned int execSize;
	unsigned int startTime;
};

LogServerInfo serverInfo;

void getTCCTime(char* out,size_t size,const char* format)
{
	DEBUG_TRY;
	time_t t = time(0);
	struct tm result;
	localtime_r(&t, &result);
	strftime( out, size, format,&result);
	DEBUG_CATCH;
}

unsigned int getLocaltimeOfHour()
{
	time_t t = time(0);
	struct tm result;
	localtime_r(&t, &result);
	return result.tm_hour;
}

unsigned int getLocaltimeOfDay()
{
	time_t t = time(0);
	struct tm result;
	localtime_r(&t, &result);
	return result.tm_mday;
}

unsigned int getLocaltimeOfMonth()
{
	time_t t = time(0);
	struct tm result;
	localtime_r(&t, &result);
	return result.tm_mon;
}

unsigned int getLocaltimeOfYear()
{
	time_t t = time(0);
	struct tm result;
	localtime_r(&t, &result);
	return result.tm_year;
}

void logServerStartTime()
{
	DEBUG_TRY;
	memset(&serverInfo, 0, sizeof(LogServerInfo));
	serverInfo.startTime = timer_get_time();
	DEBUG_CATCH;
}

void logExecSize()
{
	DEBUG_TRY;
	serverInfo.execSize++;
	DEBUG_CATCH;
}

void logNoExecSize(int size)
{
	DEBUG_TRY;
	serverInfo.noExecSize = size;
	DEBUG_CATCH;
}

void logServerInfo()
{
	DEBUG_TRY;
	static unsigned int startTime = timer_get_time();
	if(timer_get_time() - startTime > 8 * 1000)
	{
		startTime = timer_get_time();
		unsigned int serverUseTime = (timer_get_time() - serverInfo.startTime) / 1000;
		unsigned int execSizeSec = serverUseTime == 0 ? serverInfo.execSize : (serverInfo.execSize / serverUseTime);

		//BufferString bufferStr;
		//bufferStr.append(serverInfo.noExecSize).append("|").append(serverInfo.execSize).append("|").append(execSizeSec);
		//LogInfo(bufferStr.getContent(), LOG_MSC_LOGSERVER_INFO);
	}
	DEBUG_CATCH;
}


LogThread* logServer = NULL;

void *logThreadProc(void *arg) 
{
	DEBUG_TRY;
	LogThread *me = (LogThread*)arg;
	if(me)
	{
		me->loop();
	}
	return 0;
	DEBUG_CATCH;
}


void eventWriteCallBack(struct bufferevent *bev, void *arg)
{
	DEBUG_TRY;
	
	DEBUG_CATCH;
}

void eventReadCallBack(struct bufferevent *bev, void *arg)
{
	DEBUG_TRY;
	
	DEBUG_CATCH;
}

void eventErrorCallBack(struct bufferevent *bev, short what, void *arg)
{
	DEBUG_TRY;
	if(what & BEV_EVENT_CONNECTED)
	{
	}
	else if(what & BEV_EVENT_WRITING)
	{
	}
	else if(what & BEV_EVENT_READING)
	{
	}
	else
	{
	}
	DEBUG_CATCH;
}

struct bufferevent* createBufferEvent(struct event_base* base,int fd)
{
	DEBUG_TRY;
	bufferevent* event = bufferevent_socket_new(base,fd,BEV_OPT_CLOSE_ON_FREE);
	bufferevent_enable(event, EV_WRITE);
	bufferevent_setcb(event, eventReadCallBack, eventWriteCallBack, eventErrorCallBack, event);
	//bufferevent_settimeout(event,4000,4000);
	//bufferevent_setwatermark(bufferevent,EV_READ,1024,1024);
	bufferevent_setwatermark(event,EV_WRITE,0,0);
	return event;
	DEBUG_CATCH;
}

void buffereventFree(bufferevent* event)
{
	DEBUG_TRY;
	if(event)
		bufferevent_free(event);
	DEBUG_CATCH;
}

void buffereventWrite(const void* data,int size,bufferevent* event)
{
	DEBUG_TRY;
	if(event&&data&&size)
	{
		bufferevent_write(event,data,size);
	}
	DEBUG_CATCH;
}

void buffereventFlush(bufferevent* event)
{
	DEBUG_TRY;
	if(event)
	{
		bufferevent_flush(event,EV_WRITE,BEV_NORMAL);
	}
	DEBUG_CATCH;
}

void formatLogInfo(LogType type,BufferString& bufferStr,int outMaxLen,const char* info)
{
	DEBUG_TRY;
	//time
	const int maxTimeBufferSize = MAX_LOG_STRING_SIZE;
	char timeBuff[maxTimeBufferSize + 1];
	memset(timeBuff,0,maxTimeBufferSize + 1);
	getTCCTime(timeBuff,maxTimeBufferSize,TIME_YYMMDD_hhmmss);
	bufferStr.append(timeBuff);
	switch(type)
	{
	case LOG_INFO:
		bufferStr.append("|");//bufferStr.append("|INFO|");
		break;
	case LOG_WAR:
		bufferStr.append("|WAR|");
		break;
	case LOG_ERROR:
		bufferStr.append("|ERROR|");
		break;
	}
	bufferStr.append(info);
	DEBUG_CATCH;
}





LOGFILE* local_openFile(const char* filename,logWriteType writeType,struct event_base* base)
{
	DEBUG_TRY;
#ifdef LOG2FILE_WHTH_C
	FILE* fd = fopen(filename,"a");
	if(!fd)
	{
		return 0;
	}
	LOGFILE* file = (LOGFILE*)malloc(sizeof(LOGFILE));
	memset(file,0,sizeof(LOGFILE));
	file->fd = fd;
	file->type = LOG_FILE_C;
	file->createHour = getLocaltimeOfHour();
	file->createDay = getLocaltimeOfDay();
	file->createMonth = getLocaltimeOfMonth();
	file->createYear = getLocaltimeOfYear();
	return file;
#else
	int fd = open(filename,O_WRONLY|O_CREAT|O_APPEND|O_NONBLOCK,0755);
	if(!fd)
	{
		return 0;
	}
	LOGFILE* file = (LOGFILE*)malloc(sizeof(LOGFILE));
	memset(file,0,sizeof(LOGFILE));
	file->fd = fd;
	file->type = LOG_FILE_LINUX;
	file->createHour = getLocaltimeOfHour();
	file->createDay = getLocaltimeOfDay();
	file->createMonth = getLocaltimeOfMonth();
	file->createYear = getLocaltimeOfYear();
	if(writeType == LOG_WRITE_BUFFER)
	{
		file->event = createBufferEvent(base,fd);
	}
	return file;
#endif
	return 0;
	DEBUG_CATCH;
}


unsigned int isChangeLogFile(LOGFILE& file)
{
	DEBUG_TRY;
	unsigned int currHour = getLocaltimeOfHour();
	unsigned int currDay = getLocaltimeOfDay();
	unsigned int currMonth = getLocaltimeOfMonth();
	unsigned int currYear = getLocaltimeOfYear();
	if(file.createHour != currHour || file.createDay != currDay || file.createMonth != currMonth || file.createYear != currYear)
	{
		return 1;
	}
	return 0;
	DEBUG_CATCH;
}

unsigned int GetFileSize(LOGFILE& file)
{
	DEBUG_TRY;
#ifdef	LOG2FILE_WHTH_C
	if(file.type == LOG_FILE_C)
	{
		fseek(file.fd, 0, SEEK_END );
		return ftell(file.fd);
	}
#else
	if(file.type == LOG_FILE_LINUX)
	{
		struct stat file_stat;
		fstat(file.fd,&file_stat);
		return file_stat.st_size;
	}
#endif
	return 0;
	DEBUG_CATCH;
}

void CloseFile(LOGFILE* file)
{
	DEBUG_TRY;
	if(!file) return ;	
#ifdef	LOG2FILE_WHTH_C
	if(file->type == LOG_FILE_C)
	{
		if(file->fd) fclose(file->fd);
	}
#else
	if(file->type == LOG_FILE_LINUX)
	{
		if(file->fd) close(file->fd);
		buffereventFree(file->event);
	}
#endif
	free(file);
	DEBUG_CATCH;
}

void WriteFileString(const char*str,LOGFILE& file)
{
	DEBUG_TRY;
	if(str)
	{	
#ifdef	LOG2FILE_WHTH_C
		if(file.type == LOG_FILE_C)
		{
			int size = fwrite(str,strlen(str),1,file.fd);
		}
#else
		if(file.type == LOG_FILE_LINUX)
		{
			if(file.event)
			{
				buffereventWrite(str,strlen(str),file.event);
			}
			else
			{
				write(file.fd,str,strlen(str));
			}
		}
#endif
	}
	DEBUG_CATCH;
}

void WriteFileChar(char ch,LOGFILE& file)
{
	DEBUG_TRY;
	#ifdef	LOG2FILE_WHTH_C
	if(file.type == LOG_FILE_C)
	{
		fputc(ch,file.fd);
	}
#else
	if(file.type == LOG_FILE_LINUX)
	{
		if(file.event)
		{
			buffereventWrite(&ch,1,file.event);
		}
		else
		{
			write(file.fd,&ch,1);
		}
	}
#endif
	DEBUG_CATCH;
}

void FlushFile(LOGFILE& file)
{
	DEBUG_TRY;
	#ifdef	LOG2FILE_WHTH_C
	if(file.type == LOG_FILE_C)
	{
		if(file.fd)
		{
			fflush(file.fd);
		}
	}
#else
	if(file.type == LOG_FILE_LINUX)
	{
		if(file.event)
		{
			buffereventFlush(file.event);
		}
	}
#endif
	DEBUG_CATCH;
}

LOGFILE* openFile(const char* file,const char* format,logWriteType writeType,struct event_base* base)
{
	DEBUG_TRY;
	logWriteType _writetype = LOG_WRITE_DIRECT;
	if(writeType == LOG_LOCAL_NONE)
	{
#ifdef LOG2FILE_WHTH_C
		_writetype = LOG_WRITE_DIRECT;
#else
		_writetype = LOG_WRITE_BUFFER;
#endif
	}
	const int maxBuffSize = MAX_LOG_STRING_SIZE;
	char timeBuffer[maxBuffSize + 1];
	memset(timeBuffer,0,maxBuffSize + 1);
	getTCCTime(timeBuffer,maxBuffSize,format);
	BufferString bufferStr;
	bufferStr.append(getLogRootCwd()).append("/").append(file).append(".").append(timeBuffer);
	createDir(bufferStr.getContent());
	LOGFILE* fd = local_openFile(bufferStr.getContent(),_writetype,base);
	return fd;
	DEBUG_CATCH;
}

void direct_writestring(LogType type,const char* file,const char* content,unsigned size,bool format)
{
	DEBUG_TRY;
	if(!file || !content || size == 0)
		return ;
	LOGFILE* fd = openFile(file,TIME_YYMMDDHH,LOG_WRITE_DIRECT,NULL);
	if(fd)
	{
		//int size = GetFileSize(*fd);
		//if(size > MAX_LOG_FILE_SIZE)
		if(isChangeLogFile(*fd))
		{
			CloseFile(fd);
			fd = openFile(file,TIME_YYMMDDHH,LOG_WRITE_DIRECT,NULL);
		}
		WriteFileString(content,*fd);
		WriteFileChar('\n',*fd);
		FlushFile(*fd);
		CloseFile(fd);
	}
	DEBUG_CATCH;
}

bool StartLogServer()
{
	DEBUG_TRY;
	init_log();
	logServerStartTime();
	if(!logServer)
	{
		logServer = LogThread::CreateLogThread();
	}
	if(!logServer)
	{
		return false;
	}
	return true;
	DEBUG_CATCH;
}
void ExitLogServer()
{
	DEBUG_TRY;
	if(logServer)
	{
		if(logServer->stop())
		{
			delete logServer;logServer = NULL;
		}
	}
	DEBUG_CATCH;
}


void addThreadLog(int level, const char* logEvent, int lenght, const char* file)
{
	LogInfo(logEvent, file);
}

void LogDirect(const char* logEvent,const char* file,bool format)
{
	DEBUG_TRY;
	if(!file) return ;
	BufferString bufferStr;
	if(format)
	{
		formatLogInfo(LOG_INFO,bufferStr,MAX_LOG_STRING_SIZE,logEvent);
	}
	else
	{
		bufferStr.append(logEvent);
	}
	direct_writestring(LOG_INFO,file,bufferStr.getContent(),strlen(bufferStr.getContent()),format);
	DEBUG_CATCH;
}

void LogInfo(const char* logEvent,const char* file,bool format)
{
	DEBUG_TRY;
	if(!file || !logEvent) return ;
	if(logServer)
	{
		logServer->addLog(LOG_INFO,logEvent,file,format);
	}
	else
	{
	}
	DEBUG_CATCH;
}


void LogWar(const char* logEvent,const char* file,bool format)
{
	DEBUG_TRY;
	if(!file || !logEvent) return ;
	if(logServer)
	{
		logServer->addLog(LOG_WAR,logEvent,file,format);
	}
	else
	{
	}
	DEBUG_CATCH;
}


void LogError(const char* logEvent,const char* file,bool format)
{
	DEBUG_TRY;
	if(!file || !logEvent) return ;
	if(logServer)
	{
		logServer->addLog(LOG_ERROR,logEvent,file,format);
	}
	else
	{
	}
	DEBUG_CATCH;
}


LogThread::LogThread():iRun(false),iKilled(true),iEventBaseConfig(0),iEventBase(0)
{
	DEBUG_TRY;
	
	DEBUG_CATCH;
}

LogThread::~LogThread()
{
	DEBUG_TRY;
	CachedFileMap::iterator iter = iCachedFile.begin();
	while(iter != iCachedFile.end())
	{
		if(iter->second)
		{
			CloseFile(iter->second);
		}
		iter++;
	}
	iCachedFile.clear();
	if(iEventBase)
	{
		event_base_free(iEventBase);
	}
	if(iEventBaseConfig)
	{
		event_config_free(iEventBaseConfig);
	}
	DEBUG_CATCH;
}

LogThread* LogThread::CreateLogThread()
{
	DEBUG_TRY;
	LogThread* logserver = new LogThread();
	if(logserver->setupThread())
	{
		return logserver;
	}
	delete logserver;
	THROW_WITH_MSG("Setup Log Thread Fail");
	return 0;
	DEBUG_CATCH;
}

bool LogThread::setupThread()
{
	DEBUG_TRY;
	iEventBaseConfig = event_config_new();
	int error = event_config_avoid_method(iEventBaseConfig, "epoll");
	if(error != -1)
	{
		iEventBase = event_base_new_with_config(iEventBaseConfig);
	}

	pthread_mutex_init(&iFrontLogQueueLock,NULL);
	pthread_mutex_init(&iBackLogQueueLock,NULL);
	pthread_attr_t  attr;
	int             ret;
	pthread_attr_init(&attr);
	if ((ret = pthread_create(&iThreadId, &attr, logThreadProc, this)) != 0) 
	{
		THROW_WITH_MSG("LogThread::setupThread  pthread_create Fail ");
		return false;
	}
	iRun = true;
	return true;
	DEBUG_CATCH;
}

bool LogThread::stop()
{
	DEBUG_TRY;
	iRun = false;
	while(!iKilled || !iFrontLogQueue.empty() ){}
	return true;
	DEBUG_CATCH;
}


void LogThread::addLog(LogType type,const char* logEvent,const char* file /* = 0 */,bool format /*= true*/)
{
	DEBUG_TRY;
	if(!iRun)
	{
		LogDirect(logEvent,file,format);
		return ;
	}
	log_t* log = new log_t();
	log->iFile = file;
	log->iLogEvent = logEvent;
	log->iType = type;
	log->format = format;
	pthread_mutex_lock(&iBackLogQueueLock);
	//iBackFrontLogQueue.push_back(log);
	iBackFrontLogQueue.push(log);
	pthread_mutex_unlock(&iBackLogQueueLock);
	DEBUG_CATCH;
}

LogThread::log_t* LogThread::popLog()
{
	DEBUG_TRY;
	log_t* log = 0;
	pthread_mutex_lock(&iFrontLogQueueLock);
	if(!iFrontLogQueue.empty())
	{
		log = iFrontLogQueue.front();
		iFrontLogQueue.pop();
	}
	else
	{
		//copy back
		pthread_mutex_lock(&iBackLogQueueLock);
		log_t* backPacket = 0;
		while (!iBackFrontLogQueue.empty())
		{
			backPacket = iBackFrontLogQueue.front();
			iBackFrontLogQueue.pop();
			iFrontLogQueue.push(backPacket);
		}
		pthread_mutex_unlock(&iBackLogQueueLock);
	}
	pthread_mutex_unlock(&iFrontLogQueueLock);
	return log;
	DEBUG_CATCH;
}



void LogThread::writestring(log_t* log)
{
	DEBUG_TRY;
	if(!log || log->iFile.length() == 0)
	{
		return ;
	}
	LOGFILE* outFilePtr = NULL;
	const char* file = log->iFile.c_str();
	CachedFileMap::iterator iter = iCachedFile.find(file);
	if(iter != iCachedFile.end())
	{
		outFilePtr = iter->second;
		//int size = GetFileSize(*outFilePtr);
		//if(size > MAX_LOG_FILE_SIZE)
		if(isChangeLogFile(*outFilePtr))
		{
			CloseFile(outFilePtr);
			outFilePtr = openFile(file,TIME_YYMMDDHH,LOG_LOCAL_NONE,iEventBase);
			if(outFilePtr)
			{
				iCachedFile[std::string(file)] = outFilePtr;
			}
		}
	}
	else
	{
		outFilePtr = openFile(file, TIME_YYMMDDHH, LOG_LOCAL_NONE, iEventBase);
		if(outFilePtr)
		{
			iCachedFile[std::string(file)] = outFilePtr;
		}
	}
	if(!outFilePtr)
	{
		return ;
	}
	//formt
	BufferString bufferStr;
	if(log->format)
	{
		formatLogInfo(log->iType,bufferStr,MAX_LOG_STRING_SIZE,log->iLogEvent.c_str());
	}
	else
	{
		bufferStr.append(log->iLogEvent.c_str());
	}
	WriteFileString(bufferStr.getContent(),*outFilePtr);
	WriteFileChar('\n',*outFilePtr);
	FlushFile(*outFilePtr);

#ifndef  LOG2FILE_WHTH_C
	if(iEventBase)
	{
		event_base_dispatch(iEventBase);
	}
#endif // _DEBUG
	DEBUG_CATCH;
}





void LogThread::loop()
{
	//DEBUG_TRY;
	iKilled = false;
	while(true)
	{
		logServerInfo();
		log_t* log = popLog();
		if(log)
		{
			if(log->iType == LOG_STDOUT)
			{
				printf("%s\n",log->iLogEvent.c_str());
			}
			else
			{
				writestring(log);
			}
			logExecSize();
			SAFE_DELETE(log);
			//thread_sleep(10);
		}
		else
		{
			thread_sleep(100);
		}
		if(!iRun && iFrontLogQueue.empty())
		{
			break;
		}
	}
	iKilled = true;
	//DEBUG_CATCH_DUMP(LOG_APP_DUMP);
}
