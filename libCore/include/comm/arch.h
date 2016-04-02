#ifndef ARCH_H
#define ARCH_H

#include <stdlib.h>
#include "exception.h"
#include "celltype.h"
#include <map>
#include <stdio.h>
#include "Config.h"

#ifndef WIN32
#include <pthread.h>
#endif // !WIN32


class Thread
{
public:
	Thread(bool autorun=true);
	virtual ~Thread();
	void run();
	virtual void work()=0;
	void wait();
private:
#ifndef WIN32
	pthread_t id_;
	pthread_attr_t attr_;
#endif // !WIN32
};


#if defined _WIN32
#	if defined DLL_STATIC
#		define DLLEXPORT
#	elif defined DLL_EXPORT
#		define LIBCORE_EXPORT __declspec(dllexport)
#	else
#		define LIBCORE_EXPORT __declspec(dllimport)
#	endif
# endif  // !_WIN32

#define SAFE_DELETE(p)	if(p!=NULL) {delete p; p=NULL;}
#define SAFE_DELETE_ARRAY(p) if(p!=NULL) {delete[] p; p=NULL;}

#ifdef WIN32
#define strncpy strncpy_s
#define snprintf _snprintf
#define strcpy_s strcpy
#else
#define MAX_PATH 255
#endif// !WIN32


unsigned int timer_get_time();
void thread_sleep(unsigned int m);
void be_daemon();
int is_daemon();
void signalIgn();
void write_pid(const char* path);
int getFmtDataSize(const char* fmt);
int getProcessPath(char* path, int len);
int getCpuCorecCount();

#ifdef WIN32
int setenv(const char* key, const char* value, int flag);
#endif

const char* getInput();

template<typename T>
T conv_num(T value)
{
	int size = sizeof(value);
	if ( size < 2 )
		return value;

	unsigned char *p = (unsigned char *)&value;
	unsigned char *q = p+size-1;
	unsigned char tmp;
	for (int i=0; i < size/2; i++)
	{
		tmp = *p;
		*p++ = *q;
		*q-- = tmp;
	}
	return value;
}


template<typename T1, typename T2>
T2 getItem(std::map<T1,T2>& map_, T1 key)
{
	typename std::map<T1,T2>::iterator iter = map_.find(key);
	if ( iter != map_.end() )
	{
		return iter->second;
	}
	else
	{
		return NULL;
	}
	return NULL;
}

template<typename T1, typename T2>
T2 delItem(std::map<T1,T2>& map_, T1 key, bool del=false)
{
	typename std::map<T1,T2>::iterator iter = map_.find(key);
	if ( iter != map_.end() )
	{
		if(del)
			delete iter->second;
		map_.erase(iter);
		return true;
	}
	else
	{
		return false;
	}
	return true;
}


inline uint64 GetTimeDiff(uint64 newtime, uint64 oldtime)
{
	return newtime >= oldtime ? newtime - oldtime : newtime + ((uint64)(-1)-oldtime);
}

uint timeDistance(uint64 A, uint64 B);
uint64 timeStamp();



/*for logserver**/

//const char* getLogRootCwd();
bool StartLogServer();
void ExitLogServer();
void LogInfo(const char* logEvent,const char* file,bool format = true);
void LogWar(const char* logEvent,const char* file,bool format = true);
void LogError(const char* logEvent,const char* file,bool format = true);
void LogDirect(const char* logEvent,const char* file,bool format = true);
void addThreadLog(int level, const char* logEvent, int lenght, const char* file);


#define TIME_YYMMDD_hhmmss	"%Y-%m-%d %H:%M:%S"
#define TIME_YYMMDDHH$mmss	"%Y-%m-%d-%H$%M-%S"
#define TIME_YYMMDDHH		"%Y-%m-%d-%H"
#define TIME_YYMMDD			"%Y-%m-%d"
#define TIME_HH				"%H"

#define MAX_LOG_STRING_SIZE 512
#define MAX_LOG_FILE_SIZE 1024*128



//----------------------------------
#define DEBUG_TRY			\
try 						\
{
//----------------------------------
#define DEBUG_CATCH			\
}							\
catch (ExceptionBase&e)		\
{							\
	THROW_WITH_EXCEPTION(e);\
}							\
catch (std::exception&e)	\
{							\
	THROW_WITH_EXCEPTION(e);\
}							\
catch(...)					\
{							\
	THROW_WITH_MSG("Fatal Error, Catch With(...)");\
}

//-----------------------------------
#define CATCH_THROW(sMsg)	\
}							\
catch (ExceptionBase&e)		\
{							\
	THROW_WITH_MSG_EXCEPTION(sMsg,e);\
}							\
catch (std::exception&e)	\
{							\
	THROW_WITH_MSG_EXCEPTION(sMsg,e);\
}							\
catch (...)					\
{							\
	THROW_WITH_MSG(sMsg);	\
}

//---------------------------------------
#define DEBUG_CATCH_DUMP(logfile)	\
}									\
catch (ExceptionBase&e)				\
{									\
	ExceptionBase e2(__MY__FILE__,__MY__FUNCTION__,__MY__LINE__,e);\
	printf("%s", e2.what());		\
	if(logfile)						\
	{								\
		LogDirect(e2.what(), logfile);\
	}								\
}									\
catch (std::exception&e)			\
{									\
	ExceptionBase e2(__MY__FILE__,__MY__FUNCTION__,__MY__LINE__,e);\
	printf("%s", e2.what());		\
	if(logfile)						\
	{								\
		LogDirect(e2.what(), logfile);\
	}								\
}									\
catch(...)							\
{									\
	ExceptionBase e2(__MY__FILE__,__MY__FUNCTION__,__MY__LINE__,"Fatal Error, Catch With(...)");\
	printf("%s", e2.what());		\
	if(logfile)						\
	{								\
		LogDirect(e2.what(), logfile);\
	}								\
}

#endif //ARCH_H