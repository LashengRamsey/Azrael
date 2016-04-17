#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
#include "log.h"
#include "arch.h"
#include "zmq.h"
#include "comm.h"
#include <time.h>



static int id = 40001;
#define MAX_PATH_LEN 256

static int inited = 0;
static int logLevel = 0;
static bool logLocal = true;

#ifdef WIN32
#define __thread
#endif

static __thread void* zmqContext = NULL;
static __thread void* logSocket = NULL;

struct DayFile{
	DayFile()
	{
		file = NULL;
		day = 0;
	}
	FILE* file;
	int day;
};

DayFile stddf;
DayFile errdf;

std::string logpath_ = "log/";
std::string logName_ = "log/game";

int createDir(const char *_pszDir)
{
	DEBUG_TRY;
	char pszDir[512];
	strcpy(pszDir,_pszDir);
	unsigned int i = 0;
	unsigned int iRet;
	unsigned int iLen = strlen(pszDir);
	if (pszDir[iLen - 1] != '\\' && pszDir[iLen - 1] != '/')
	{
		pszDir[iLen] = '/';
		pszDir[iLen + 1] = '\0';
	}
	for (i = 0;i < iLen;i ++)
	{
		if (pszDir[i] == '\\' || pszDir[i] == '/')
		{ 
			pszDir[i] = '\0';
			iRet = ACCESS(pszDir,0);
			if (iRet != 0)
			{
				iRet = MKDIR(pszDir);
				if (iRet != 0)
				{
					//return -1;
				} 
			}
			pszDir[i] = '/';
		} 
	}
	return 0;
	DEBUG_CATCH;
}

char logRootCWd[MAX_LOG_ROOT_CWD_SIZE]={0};
const char* getLogRootCwd()
{
	DEBUG_TRY;
	if( strlen(logRootCWd) == 0 )
	{
		//getcwd(logRootCWd, MAX_LOG_ROOT_CWD_SIZE);
		strncat(logRootCWd,"../bin/log", MAX_LOG_ROOT_CWD_SIZE - strlen(logRootCWd));
	}
	return logRootCWd;
	DEBUG_CATCH;
}

void add_log(int source, int priority, const char* pMsg, int msgisze);
void init_log()
{
	if (inited)
		return;

	//createDir(logpath_.c_str());

	assert(zmqContext == NULL);
	assert(logSocket == NULL);

	id = Config::GetIntValue("ServerID");
	if (id)
	{
		logName_.clear();
		logName_.append("game.");
		logName_ += Config::GetValue("ServerID");
		logName_.append("/debug/debug");

	}
	
	logLevel = Config::GetIntValue("LogLevel");

	zmqContext = zmq_init(1);
	logSocket = zmq_socket(zmqContext, ZMQ_DEALER);

	const char *logpath = Config::GetValue("LogPath");
	assert(logpath);

	if (is_daemon())
	{
	}

	char path[MAX_PATH];
	snprintf(path, MAX_PATH, "ipc://%s/log.sock", logpath);
	printf("init log, id:%d, path:%s\n", id, path);
	
	zmq_setsockopt(logSocket, ZMQ_IDENTITY, &id, sizeof(id));
	uint16_t hwm = 1000;
	zmq_setsockopt(logSocket, ZMQ_SNDHWM, &hwm, sizeof(uint16_t));

	zmq_connect(logSocket, path);

	inited = 1;
	LOG("Connect logserver %s", path);
}


void fini_log()
{
	int zero = 0;
	if (logSocket)
	{
		zmq_setsockopt(logSocket, ZMQ_LINGER, &zero, sizeof(zero));
		zmq_close(logSocket);
		zmq_term(zmqContext);
		logSocket = NULL;
		zmqContext = NULL;

	}
}


void freebuffer(void* data, void* hint)
{
	free(data);
}

#define LOGMAX 65535
void PrintLog(int level, const char* fmt, ...)
{
	//init_log();
	if (logSocket && level > logLevel)
	{
		char *buffer = (char*)malloc(LOGMAX+1);
		buffer[0] = level;
		
		va_list va;
		va_start(va, fmt);
#ifdef WIN32
		int n = vsnprintf(buffer+1, LOGMAX, fmt, va);
#else
		int n = vsnprintf(buffer+1, LOGMAX, fmt, va);
#endif
		va_end(va);

		n = n > LOGMAX ? LOGMAX : n;
		if (logLocal)
		{
			addThreadLog(level, buffer+1, n, logName_.c_str());
			//add_log(id, level, buffer+1, n);
		}

		zmq_send(logSocket, buffer, n+1, ZMQ_NOBLOCK);
		free(buffer);
	}
}


void DumpMemory(void *src, int len)
{
	int lc = 16;
	int pos = 0;
	int line = 0;
	int tmplen;
	int i;
	unsigned int x;
	char node[4];
	char val[256] = {0,};
	char part3[16+1] = {0,};

	for (; len > 0; len -= lc)
	{
		memset(val, 0, sizeof(val));
		memset(part3, 0, sizeof(part3));
		sprintf(val, "%08X : ", line);

		tmplen = len;
		if (tmplen > lc)
			tmplen = lc;

		for ( i = 0; i < tmplen; ++i)
		{
			x = *(unsigned char*)((unsigned char*)src + pos);
			pos++;
			*((unsigned int*)(void*)node) = 0;
			sprintf(node, "%02X", x);
			strcat(val, node);

			memset(node, 0, sizeof(node));
			if (x >= '!' && x <= '-')
			{
				node[0] = x;
			}
			else
			{
				node[0] = '.';
			}
			strcat(part3, node);
		}
		for (; i < lc; ++i)
		{
			strcat(val, "    ");
		}
		strcat(val, " ; ");
		strcat(val, part3);
		LOG("%s\n", val);
		line++;
	}
}


inline int dayInteger(struct tm* now)
{
	return (now->tm_yday + 1900)*10000
		+ (now->tm_mon + 1)*100
		+ now->tm_mday;
}

bool check_new_day(struct tm* now, int day)
{
	int newday = dayInteger(now);
	return newday != day;
}


FILE* open_log_file(DayFile* dayfile, const char* log_path, const char* log_name)
{
	time_t t = time(NULL);
	struct tm* now = localtime(&t);

	if (dayfile->file != NULL)
	{
		if (!check_new_day(now, dayfile->day))
			return dayfile->file;
	}

	if (dayfile->file == NULL)
	{
		char log_filepath[MAX_PATH_LEN] = "";

		strcpy(log_filepath, log_path);
		strcat(log_filepath, log_name);

		char date_buf[30];
		sprintf(date_buf, ".%d-%d-%d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
		strcat(log_filepath, date_buf);
		
		dayfile->file = fopen(log_filepath, "a");
		if (dayfile->file == NULL)
		{
			printf("log fopen error errno = %d, code = %s", errno, strerror(errno));
			return NULL;
		}

		dayfile->day = dayInteger(now);
		return dayfile->file;
	}
	
	fclose(dayfile->file);
	dayfile->file = NULL;
	dayfile->day = dayInteger(now);

	t = time(NULL);
	now = localtime(&t);
	char new_filepath[MAX_PATH_LEN] = "";
	strcpy(new_filepath, log_path);
	strcat(new_filepath, log_name);

	char date_buf[30];
	sprintf(date_buf, ".%d-%d-%d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
	strcat(new_filepath, date_buf);

	dayfile->file = fopen(new_filepath, "a");
	return dayfile->file;
}


void add_log(int source, int priority, const char* pMsg, int msgsize)
{
	time_t nowtime = time(NULL);
	struct tm *local = localtime(&nowtime);
	char nowtimes[128];

	strftime(nowtimes, 128, "%c", local);

	if (priority >= 0)
	{
		fprintf(stdout, "%s\n", pMsg);
		fflush(stdout);
	}
	FILE *stdfile = open_log_file(&stddf, logpath_.c_str(), logName_.c_str());
	if (stdfile)
	{
		fprintf(stdfile, "%s [%d]:%.*s\n", nowtimes, source, msgsize, pMsg);
		fflush(stdfile);
	}
}





