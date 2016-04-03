#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

const int L_LOG=1;
const int L_INFO=2;
const int L_NOTICE=3;
const int L_WARNING=4;
const int L_ERROR=5;
const int L_CRITICAL=6;

void init_log();
void fini_log();
void PrintLog(int level, const char* fmt, ...);

#ifdef WIN32
#define LOG(fmt, ...) PrintLog(L_LOG, fmt, __VA_ARGS__);
#define INFO(fmt, ...) PrintLog(L_INFO, fmt, __VA_ARGS__);
#define ERROR(fmt, ...) PrintLog(L_ERROR, fmt, __VA_ARGS__);
#define NOTICE(fmt, ...) PrintLog(L_NOTICE, fmt, __VA_ARGS__);
#define CRITICAL(fmt, ...) PrintLog(L_CRITICAL, fmt, __VA_ARGS__);
#define WARNING(fmt, ...) PrintLog(L_WARNING, fmt, __VA_ARGS__);
#else
#define LOG(fmt, args...) PrintLog(L_LOG, fmt, ##args);
#define INFO(fmt, args...) PrintLog(L_INFO, fmt, ##args);
#define ERROR(fmt, args...) PrintLog(L_ERROR, fmt, ##args);
#define NOTICE(fmt, args...) PrintLog(L_NOTICE, fmt, ##args);
#define CRITICAL(fmt, args...) PrintLog(L_CRITICAL, fmt, ##args);
#define WARNING(fmt, args...) PrintLog(L_WARNING, fmt, ##args);
#endif

void DumpMemory(void* src, int len);

#define MARK INFO("MARK: %-4d:%s %s", __LINE__, __FILE__, __FUNCTION__)
#define FATAL(tip) {ERROR("ERROR: %s %s %d:%s\n", __FUNCTION__, __LINE__, __FILE__, tip);}
#define ASSERT(cond) {if(!(cond)) {ERROR("ASSERT failed on: %s %s %d", __LINE__, __FUNCTION__, __FILE__);abort();}}


#ifdef WIN32
#include <io.h>
#include <direct.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#ifdef WIN32
#define ACCESS _access
#define MKDIR(a) _mkdir((a))
#else
#define ACCESS access
#define MKDIR(a) mkdir((a),0755)
#endif

#define MAX_LOG_ROOT_CWD_SIZE 256

int createDir(const char *_pszDir);
const char* getLogRootCwd();

#endif //LOG_H

