#include "app.h"
#include "lua.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <cstddef>
#include <assert.h>
#include <time.h>
#include "arch.h"
#include "mqnet.h"
#include "log.h"
#include "net.h"
#include "luasvr.h"
#include "timer.h"
#include "msg.h"
#include "Config.h"


#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <link.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <err.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <syslog.h>
#include <execinfo.h>
#endif

ServerApp *ServerApp::Self_ = NULL;

int ServerApp::loadConfig(const char *file)
{
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	std::string path(file);
	size_t pos = path.find_last_of("/");

	if (pos != std::string::npos)
	{
		path = path.substr(0, pos);
		lua_pushstring(L, path.c_str());
		lua_setglobal(L, "ConfigPath");
	}

	if (luaL_dofile(L, file))
	{
		ERRLOG("%s", lua_tostring(L, -1));
		return 1;
	}

	int idxG;
	char *key = NULL;
	char *val = NULL;
	lua_getglobal(L, "Config");
	if (!lua_istable(L, -1))
		return 1;

	idxG = lua_gettop(L);
	lua_pushnil(L);
	//lua_next先把 表(lua栈 index所指的表), 的当前索引弹出，再把table 当前索引的值弹出
	//这里重点说明一下lua_next。它执行操作是这样的，先判断上一个key的值
	//（这个值放在栈顶，如果是nil，则表示当前取出的是table中第一个元素的值），
	//然后算出当前的key，这时先把栈顶出栈，将新key进栈，最后将新key对应的值进栈。
	//这样栈顶就是table中第一个遍历到的元素的值。
	//用完这个值后，我们要把这个值出栈，让key在栈顶以便继续遍历。当根据上一个key值算不出下一个key值时，lua_next返回0，结束循环。

	while(lua_next(L, idxG) != 0)
	{
		key = (char*)lua_tostring(L, -2);
		val = (char*)lua_tostring(L, -1);
		setenv(key, val?val:"", 1);
		lua_pop(L, 1);
	}
	lua_close(L);
	return 0;
}


ServerApp::ServerApp()
	:mqnet_(NULL)
	,net_(NULL)
	,timer_(NULL)
	,lua_(NULL)
{
	appResume_ = 1;
	Self_ = this;
}

ServerApp::~ServerApp()
{
	LOG("ServerApp exit");
}

ServerApp* ServerApp::get()
{
	return Self_;
}

#ifndef WIN32
static void sh(int sig)
{
	if (SIGINT == sig || SIGTERM == sig)
	{
		ServerApp::get()->stop();
	}

	if (SIGUSR1 == sig)
	{
		LOG("attemp to load update lua script!");
		LuaSvr::get()->setScriptUpdate();
	}
}

static __sighandler_t oldsigsegv = 0;
//在程序出错时打印出函数的调用堆栈
void sigdump(int s)
{
	ERRLOG("App segment fault");
	void *array[10];
	size_t size = 0;
	char **strings;
	size_t i;

	size = backtrace (array, 10);
	strings = backtrace_symbols (array, size);
	ERRLOG("Obtained %2d stack fames.\n", size);
	for (i = 0; i < size; ++i)
	{
		ERRLOG("%s\n", strings[i]);
	}
	free(strings);
	oldsigsegv(s);
	exit(-1);

}

static void s_catch_signals(void)
{
	signalIgn();
	struct sigaction action;
	action.sa_handler = sh;
	action.sa_flags = 0;

	sigemptyset(&action.sa_mask);
	sigaction(SIGINT, &action, NULL);
	sigaction(SIGUSR1, &action, NULL);
	sigaction(SIGUSR2, &action, NULL);
	sigaction(SIGTERM, &action, NULL);
	
	oldsigsegv = signal(SIGSEGV, sigdump);
}
#endif

void ServerApp::setupSignal()
{
#ifndef WIN32
	s_catch_signals();
#endif
}

void ServerApp::rlimit()
{
#ifndef WIN32
	struct rlimit olimit;
	if (!getrlimit(RLIMIT_CORE, &olimit))
	{
		NOTICE("<<rlimit core cur=%d,max=%d", olimit.rlim_cur, olimit.rlim_max);
		olimit.rlim_cur = olimit.rlim_max = RLIM64_INFINITY;
		if (!setrlimit(RLIMIT_CORE, &olimit))
		{
			if (!getrlimit(RLIMIT_CORE, &olimit))
			{
				NOTICE(">>RLIMIT core cur=%d,max=%d", olimit.rlim_cur, olimit.rlim_max);
			}
		}
	}

	if (!getrlimit(RLIMIT_NOFILE, &olimit))
	{
		NOTICE(">>RLIMIT maxfd cur=%d,max=%d", olimit.rlim_cur, olimit.rlim_max);
		
		olimit.rlim_cur = olimit.rlim_max = 10240;
		if (!setrlimit(RLIMIT_NOFILE, &olimit))
		{
			if (!getrlimit(RLIMIT_NOFILE, &olimit))
			{
				NOTICE(">>RLIMIT maxfd cur=%d,max=%d", olimit.rlim_cur, olimit.rlim_max);
				if(olimit.rlim_cur != 10240)
				{
					ERRLOG("Can't set maxfd to 10240, it's %s", olimit.rlim_cur);
				}
			}
		}
		else
		{
			ERRLOG("Can't change maxfd to proper value, should fix this problem");
		}
	}
#endif
}


bool ServerApp::init(int argc, char* argv[])
{
	//if (argc<3)
	//{
	//	INFO("argc = %d,not Config", argc);
	//	return false;
	//}
	
	char* s = getenv("ServerID");
	const char* dir = Config::GetValue("ScriptDir");
	if (dir && chdir(dir))
	{
		printf("Failed, no define ScirptDir dir = %s\n", dir);
		ERRLOG("Failed, no define ScirptDir");
		return false;
	}

	//配置文件路径
	//Config::SetConfigName(argv[1]);
	INFO("Config path:		%s",argv[1]);
	
	myName_ = Config::GetValue("ServerID");
	sockPath_ = Config::GetValue("SocketPath");

	//if (loadConfig(Config::GetValue("LogPath")))
	//{
	//	printf("Load Config %s failed ", Config::GetValue("LogPath"));
	//}

	int daemon = Config::GetIntValue("Daemon");
	if (daemon)
	{
		be_daemon();
	}

	setupSignal();

	//open core dump
	rlimit();
	
	//write pid to file
	const char *pidFile = Config::GetValue("PidFile");
	if (!pidFile)
	{
		ERRLOG("get config pidFile error");
		return false;
	}
	INFO("load config %s ok!", pidFile);
	write_pid(pidFile);
	INFO("Load config %s", pidFile);
	//randomized seed by time
	srand((unsigned int)time(NULL));
	//tick_timer();

	lastTickTime_  = timer_get_time();
	
	if (!Config::GetValue("RouterPort"))
	{
		ERRLOG("Failed, no define RouterPort");
		return false;
	}
	//init zmq
	createContext();
	return true;
}


void ServerApp::start()
{
	run();
	loop();
	fini();
}

void ServerApp::stop()
{
	appResume_ = 0;
	if (mqnet_)
	{
		mqnet_->disconnect();
	}
}

void ServerApp::run()
{
	onInited();
	//初始化lua脚本
	if(lua_)
	{
		lua_->init();
	}
	//网络层，客户端
	if(net_)
	{
		net_->init();
	}
	//运行lua脚本
	if(lua_)
	{
		lua_->run();
	}
	//定时器
	if(timer_)
	{
		timer_->init();
	}
	INFO("Server start success");
}


void ServerApp::update()
{
	int dtime = timer_get_time() - lastTickTime_;
	if (dtime<0)
	{
		ERRLOG("calcd dtime < 0 ,time %d,lastTick %d", timer_get_time(), lastTickTime_);
		dtime  = 0;
	}
	//定时器
	if(timer_)
	{
		timer_->update(dtime);
	}
	//zmq消息
	if(mqnet_)
	{
		mqnet_->update(dtime);
	}
	//网络update
	if(net_)
	{
		net_->update(dtime);
	}
	//lua脚本update
	if(lua_)
	{
		lua_->doUpdate(dtime);
	}

	onUpdate(dTime_);

	//提取网络消息
	if(net_ && net_->mode()&LM_LOOP)
	{
		event_base_dispatch(get_default_evbase());
	}
	else
	{
		event_base_loop(get_default_evbase(), EVLOOP_NONBLOCK);
	}

	lastTickTime_ = timer_get_time();
	dTime_ = dtime;
}

//收到ZMQ连接网络消息
void ServerApp::doMqMsg(int target, int fid, Buf *buf)
{
	uint sn = 0;
	
	*buf >> sn;

	if(PT_CLIENT_OFFLINE == fid)
	{
		LuaSvr::call("doDisconnect", "ii", target, sn);
	}
	else if(PT_NETTEST_MSG == fid)
	{
		*buf << timer_get_time();
		if(mqnet_)
		{
			mqnet_->methodTo(target, fid, sn, *buf);
		}
	}

	std::string data;
	buf->readText(data);

	LuaSvr::call("CHandlerMsg", "iiiSii", target, sn, fid, &data, 0, data.size());
}

//收到客户端连接网络消息
void ServerApp::doNetMsg(int sn, Buf *buf)
{
	if (buf == NULL)
	{
		ERRLOG("doNetMsg method buf is NULL error");
		return;
	}
	int len = buf->getLength();
	if (len < 2)
	{
		ERRLOG("doNetMsg method len less then 2 error");
		return;
	}
	//uint16 fid = 0;
	//*buf >> fid;
	std::string data;
	buf->readText(data);
	//LuaSvr::call("CHandlerNetMsg", "iiSii", sn, fid, &data, 0, data.size());
	LuaSvr::call("CHandlerNetMsg", "iSii", sn, &data, 0, data.size());
}

//轮询
void ServerApp::loop()
{
	while(true)
	{
		if(!appResume_)
			break;
		update();
		if(dTime_<1)
		{
			thread_sleep(1);
		}
	}
}

//分割字符串
typedef std::vector<std::string> StringVec;
void split(const char *str, StringVec& keys, const char* separators)
{
	if (!str)
		return;

	char *resToken;
	resToken = strtok((char*)str, separators);
	while( resToken != 0)
	{
		keys.push_back(resToken);
		resToken = strtok(0, separators);
	}
}


//连接router
void ServerApp::connectRouter()
{
	if(mqnet_)
	{
		StringVec routerPorts;
		split(Config::GetValue("RouterPort"), routerPorts, ";");

		for (StringVec::iterator iter = routerPorts.begin();
			iter != routerPorts.end(); iter++)
		{
			char addr[MAX_PATH];
			snprintf(addr, MAX_PATH, "tcp://%s", iter->c_str());
			/*if (strcmp(sockPath_, "tcp://") >= 0)
			{
				snprintf(addr, MAX_PATH, "%s:%s", sockPath_, iter->c_str());
			}
			else
			{
				snprintf(addr, MAX_PATH, "%s/router_%s", sockPath_, iter->c_str());
			}*/
			INFO("connectRouter		addr = %s", addr);
			mqnet_->connect(atoi(myName_), addr);
		}
	}
	else
	{
		ERRLOG("Failed, ServerApp::connectRouter mqnet_ is null");
	}
}

//连接db服务器
void ServerApp::connectDB()
{
	if(mqnet_)
	{
		StringVec DBSockets;
		split(Config::GetValue("DBId"), DBSockets, ";");

		for (StringVec::iterator iter = DBSockets.begin();
			iter != DBSockets.end(); iter++)
		{
			char addr[MAX_PATH];
			snprintf(addr, MAX_PATH, "tcp://%s", iter->c_str());
			INFO("connectDB		addr = %s", addr);
			mqnet_->connectDB(addr);
		}
	}
	else
	{
		ERRLOG("Failed, ServerApp::connectDB mqnet_ is null");
	}
}

//连接router、db，只有game有用
void ServerApp::connect()
{
	connectRouter();
	connectDB();
}


void ServerApp::fini()
{
	LOG("delete mqnet");
	SAFE_DELETE(mqnet_);
	LOG("delete eventnet");
	SAFE_DELETE(net_);
	LOG("delete luasvr");
	SAFE_DELETE(lua_);
	SAFE_DELETE(timer_);
}

void ServerApp::createContext()
{
	mqnet_ = new MQNet();
	lua_ = new LuaSvr();
	timer_ = new Timer();
}

unsigned int ServerApp::getServerID()
{
	return atoi(myName_);
}

int ServerApp::SendPacket(int target, int fid, int sn, const Buf &buf)
{
	if (target == -1)
	{
		Net *net = getNet();
		if(net)
		{
			net->sendPacket(sn, fid, buf);
		}
	}
	else
	{
		MQNet *mqnet = getMQNet();
		if (mqnet)
		{
			INFO("[lua proto]SendPacket sned msg, target:%d, fid:%d, sn:%d, data size:%d", target, fid, sn, buf.getLength());
			mqnet->methodTo(target, fid, sn, buf);
		}
	}
	return 0;
}


