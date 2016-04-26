#include "arch.h"
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "celltype.h"

#ifdef WIN32
#include <Windows.h>
#include <process.h>
#else
#include <sys/time.h>
#include <sys/ioctl.h>
#endif


static int g_isDaemon = 0;

void *stack(int i)
{
#ifndef WIN32
	switch(i)
	{
	case 0:
		return __builtin_return_address(1);
	case 1:
		return __builtin_return_address(2);
	case 2:
		return __builtin_return_address(3);
	case 3:
		return __builtin_return_address(4);
	case 4:
		return __builtin_return_address(5);
	case 5:
		return __builtin_return_address(6);
	default:
		return 0;
	}
#endif
	return 0;
}


void resetIO()
{
#ifndef WIN32
	if fd;
	for (int i = 0; i < 3; ++i)
	{
		close(i);
	}

	fd = open("/dev/null", O_RDWR);
	if (fd != STDERR_FILENO)
	{
		dup2(fd, STDERR_FILENO);
		close(fd);
	}

	fd = open("/dev/null", O_RDWR);
	if (fd != STDIN_FILENO)
	{
		dup2(fd, STDIN_FILENO);
		close(fd);
	}

	fd = open("/dev/null", O_RDWR);
	if (fd != STDOUT_FILENO)
	{
		dup2(fd, STDOUT_FILENO);
		close(fd);
	}
#endif
}


void be_daemon()
{
#ifndef WIN32
	umask(0);
	pid_t pid;
	if ((pid = fork()) != 0)
	{
		exit(0);
	}
	setsid();
	if ((pid = fork()) != 0)
	{
		exit(0);
	}
	resetIO();
	g_isDaemon = 1;
#endif
}

void signalIgn()
{
#ifndef WIN32
	signal(SIGTERM, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
#endif
}

int is_daemon()
{
	return g_isDaemon;
}

int getFmtDataSize(const char* fmt)
{
	int need = 0;
	for (uint i = 0; i < strlen(fmt); ++i)
	{
		char c;
		switch((c=fmt[i]))
		{
		case 'i':
		case 'I':
		case 'f':
			need += 4;
			break;
		case 'h':
		case 'H':
			need += 2;
			break;
		case 'b':
			need += 1;
			break;
		default:
			return 0;
		}
	}
	return need;
}


int getProcessPath(char *path, int len)
{
#ifndef WIN32
	char cmd[MAX_PATH] = {0,};
	FILE* fp = NULL;

	sprintf(cmd, "readlink -f /proc/%d/exe", getpid());
	if (NULL == (fp=popen(cmd, "r")))
	{
		return 1;
	}
	if (NULL == fgets(path, len, fp))
	{
		pclose(fp);
		return 1;
	}
	pclose(fp);
	if (path[strlen(path)-1] == '\n')
		path[strlen(path)-1] = 0;
#endif
	return 0;
}


int getCpuCorecCount()
{
#ifndef WIN32
	FILE *fp;
	char res[128] = {0,};
	fp = popen("/bin/cat /proc/cpuinfo | grep -c '^processor'", "r");
	fread(res, 1, sizeof(res)-1, fp);
	fclose(fp);
	return atoi(res);
#endif
	return 0;
}

void write_pid(const char* path)
{
	char buff[128] = {0,};
	snprintf(buff, sizeof(buff), "%s/pid", path);
	printf("wirte pid at path:%s\n", buff);
#ifndef WIN32
	FILE *fp = fopen(buff, "w+");
	if (fp)
	{
		fprintf(fp, "%d", getpid());
		fclose(fp);
		if (chmod(buff, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH))
		{
			perror("what");
		}
	}
#endif
}

uint64 timeStamp()
{
#ifndef WIN32
	static u_int64_t tsStart = 0;
	struct timespec tsNow;
	clock_gettime(CLOCK_REALTIME, &tsNow);
	uint64 now = tsNow.tv_sec*1000000000L + tsNow.tv_nsec;
	if (tsStart == 0)
		tsStart = now;
	return now-tsStart;
#endif
	return 20;
}

uint timeDistance(uint64 A, uint64 B)
{
	uint c = (B - A)*0.000001f;
	return c;
}

unsigned int timer_get_time()
{
	DEBUG_TRY;
	static bool is_start = false;
#ifdef WIN32
	return GetTickCount();
#else
#if HAVE_CLOCK_GETTIME
	static struct timespec start;
	if (!is_start)
	{
		clock_gettime(CLOCK_MONOTONIC, &start);
		is_start = true;
	}
	unsigned int ticks;
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	ticks = (now.tv_sec - start.tv_sec)*1000 + (now.tv_nsec - start.tv_nsec)/1000;
	return ticks;
#else
	static struct timespec start;
	if (!is_start)
	{
		gettimeofday(&start, NULL);
		is_start = true;
	}
	unsigned int ticks;
	struct timespec now;
	gettimeofday(&now, NULL);	//使用C语言编写程序需要获得当前精确时间（1970年1月1日到现在的时间）
	ticks = (now.tv_sec - start.tv_sec)*1000 + (now.tv_nsec - start.tv_nsec)/1000;
	return ticks;
#endif
#endif
	DEBUG_CATCH;
}


void thread_sleep(unsigned int m)
{
	DEBUG_TRY;
#ifdef WIN32
	Sleep(m);
#else
	usleep(m * 1000);
#endif
	DEBUG_CATCH;
}

#ifdef WIN32
int setenv(const char* key, const char* value, int flag)
{
	char buffer[40960] = {0};
	snprintf(buffer, sizeof(buffer), "%s=%s", key, value);
	return _putenv(buffer);
}
#endif

#ifdef WIN32
#include <Windows.h>
#define BUFFER_MAX 1024

char g_nbstdin_buffer[2][BUFFER_MAX];

HANDLE g_input[2];
HANDLE g_process[2];

DWORD WINAPI console_input(LPVOID lpParameter)
{
	for (;;)
	{
		int i;
		for (i = 0;i<2;++i)
		{
			fgets(g_nbstdin_buffer[i], BUFFER_MAX, stdin);
			SetEvent(g_input[i]);
			WaitForSingleObject(g_process[i], INFINITE);
		}
	}
	return 0;
}

void create_nbstdin()
{
	int i;
	DWORD tid;
	CreateThread(NULL, 1024, &console_input, 0, 0, &tid);
	for (i = 0;i<2;++i)
	{
		g_input[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		g_process[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		g_nbstdin_buffer[i][0] = '\0';
	}
}

const char *nbstdin()
{
	DWORD n = WaitForMultipleObjects(2, g_input, FALSE, 0);
	if (n == WAIT_OBJECT_0 || n == WAIT_OBJECT_0 + 1)
	{
		n = n - WAIT_OBJECT_0;
		SetEvent(g_process[n]);
		return g_nbstdin_buffer[n];
	}
	else
	{
		return 0;
	}
}

#endif


//获取控制台输入
const char* getInput()
{
	static int init = 0;
#ifdef WIN32
	if (init == 0)
	{
		create_nbstdin();
		init = 1;
	}
	return nbstdin();
#else
	static char buff[512] = {0};
	if (init == 0)
	{
		unsigned long noblock = 1;
		ioctl(STDIN_FILENO, FIONBIO, &noblock);
	}
	int len = read(STDIN_FILENO, buff, sizeof(buff));
	if (len <= 0)
		return NULL;
	buff[len] = '\0';
	return buff;
#endif
}


//==================================================
//线程相关
#ifdef WIN32
void __cdecl threadTrample(void * ud)
#else
void * threadTrample(void * ud)
#endif
{
	Thread *th = (Thread*)ud;
	if (th)
	{
		th->work();
	}
#ifndef WIN32
	//return NULL;
#endif
}



Thread::Thread(bool autorun)
{
	if (autorun)
	{
		run();
	}
}

Thread::~Thread()
{
#ifndef WIN32
	pthread_attr_destroy(&attr_);
	if (id_)
	{
		pthread_join(id_, NULL);
	}
#endif
}

void Thread::run()
{
#ifdef WIN32
	_beginthread(&threadTrample, 0, this);
#else
	pthread_attr_init(&attr_);
	pthread_create(&id_, &attr_, threadTrample, this);
#endif
}

void Thread::wait()
{
#ifndef WIN32
	if (id_)
	{
		pthread_join(id_, NULL);
		id_ = 0;
	}
#endif
}
//==================================================
