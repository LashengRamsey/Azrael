#include <string.h>
#include <fcntl.h>
#include "luasvr.h"
#include "log.h"
#include "net.h"
#include "app.h"
#include <time.h>
#include "luaglobal.h"
#include "arch.h"
#include "luabit.h"
#include "timer.h"
#include "connection.h"
#include "luanetwork.h"


#define LuaDebug

LuaSvr* LuaSvr::luaSvrSelf_ = NULL;


static int gRef[REF_MAX];

typedef std::map<std::string, int> RefMap;
RefMap gRefMap;

//�ű����ִ��ʱ��
static uint maxScriptTime = 10000;

class ScriptTimeCheckThread : public Thread{
public:
	ScriptTimeCheckThread(lua_State *L):Thread(false)
	{
		L_ = L;
		enterTimer_ = 0;
		level_ = 0;
		break_ = 0;
		run();
	}

	void enter()
	{
		if(level_ == 0)
			enterTimer_=(uint)time(NULL);
		level_++;
	}

	void leave()
	{
		level_--;
		if(level_==0)
			enterTimer_=0;
	}

	static void timeoutBreak(lua_State *L, lua_Debug *D)
	{
		lua_sethook(L, NULL, 0, 0);
		luaL_error(L, "Script timeout over 10 seconds.");
	}

	virtual void work()
	{
		int enterTime = 0;
		while(true)
		{
			thread_sleep(1000);
			enterTime = enterTimer_;
			if (enterTime && maxScriptTime && time(NULL)-enterTime > maxScriptTime)
			{
				int mask = LUA_MASKCALL | LUA_MASKRET | LUA_MASKLINE | LUA_MASKCOUNT;
				int ret = lua_sethook(L_, timeoutBreak, mask, 1);
				printf("check script time out:%d-%d-%d", (uint)time(NULL), enterTime, level_);
				enterTimer_ = 0;
			}
			if (break_)
				break;
		}
	}

	void terminate()
	{
		break_ = 1;
	}

private:
	lua_State* L_;
	uint enterTimer_;
	uint level_;
	uint break_;
};



ScriptTimeCheckThread* LuaSvr::scriptCTT_ = NULL;
//�ű�������Ϣ
//panic �������Դ�ջ��ȡ��������Ϣ
static int error_hook(lua_State *L)
{
	ERROR("[LUA ERROR] %s", lua_tostring(L, -1));
	lua_Debug ldb;
	int i = 0;
	std::string errdump = lua_tostring(L, -1);
	errdump += "\n";
	
	for (i = 0; lua_getstack(L, i, &ldb) == 1; i++)
	{
		lua_getinfo(L, "Slnu", &ldb);
		const char *name = ldb.name ? ldb.name : "";
		const char *filename = ldb.source ? ldb.source : "";
		char errline[8096];
		snprintf(errline, 8096, "[LUA ERROR] %s '%s' @'%s:%d'\n",
			ldb.what, name, filename, ldb.currentline);
		errdump += errline;
	}
	ERROR(errdump.c_str());
	ERROR("=======Report error to server========");

	//���ýű�
	LuaSvr::call("CHandlerError", "S", &errdump);

	return 0;
}

//Lua �ṩ��һ��ע�������һ��Ԥ��������ı�
//�������������κ� C �����뱣��� Lua ֵ��
//����������α���� LUA_REGISTRYINDEX ����λ��
//�κ� C �ⶼ���������ű��ﱣ�����ݣ�Ϊ�˷�ֹ��ͻ������Ҫ�ر�С�ĵ�ѡ�������
//һ����÷��ǣ��������һ��������Ŀ������ַ�����Ϊ������
//���߿���ȡ���Լ� C �����е�һ����ַ���� light userdata ����ʽ����
void ref(lua_State *L, const char *fn, int r)
{
	gRef[r] = luaL_ref(L, LUA_REGISTRYINDEX);
	gRefMap[fn] = gRef[r];
}

//��ȡ�ű�����
void LuaSvr::initRef()
{
	for (int i = 0; i < REF_MAX; ++i)
	{
		gRef[i] = LUA_NOREF;
	}

	lua_getglobal(L_, "CHandlerTimer");
	if (lua_isnil(L_, -1))
	{
		FATAL("[LUA FATAL] lua script no CHandlerTimer function****");
	}
	ref(L_, "CHandlerTimer", REF_DO_TIMER);

	lua_getglobal(L_, "CHandlerMsg");
	if (lua_isnil(L_, -1))
	{
		FATAL("[LUA FATAL] lua script no CHandlerMsg function****");
	}
	ref(L_, "CHandlerMsg", REF_DO_MSG);

	lua_getglobal(L_, "CHandlerConnect");
	if (lua_isnil(L_, -1))
	{
		FATAL("[LUA FATAL] lua script no CHandlerConnect function****");
	}
	ref(L_, "CHandlerConnect", REF_CONNECT);

	lua_getglobal(L_, "CHandlerDisconnect");
	if (lua_isnil(L_, -1))
	{
		FATAL("[LUA FATAL] lua script no CHandlerDisconnect function****");
	}
	ref(L_, "CHandlerDisconnect", REF_DISCONNECT);

	lua_getglobal(L_, "CHandlerError");
	if (lua_isnil(L_, -1))
	{
		FATAL("[LUA FATAL] lua script no CHandlerError function****");
	}
	ref(L_, "CHandlerError", REF_ERROR);

	lua_getglobal(L_, "CHandlerNetMsg");
	if (lua_isnil(L_, -1))
	{
		FATAL("[LUA FATAL] lua script no CHandlerNetMsg function****");
	}
	ref(L_, "CHandlerNetMsg", REF_ERROR);
}

int LuaSvr::getRef( int ref)
{
	if (ref > 0 && ref < REF_MAX)
	{
		if (gRef[ref] != LUA_NOREF && gRef[ref] != LUA_REFNIL)
		{
			lua_rawgeti(L_, LUA_REGISTRYINDEX, gRef[ref]);
			if (!lua_isnil(L_, -1))
				return 0;
		}
	}
	return 1;
}

int LuaSvr::getRef(const char* fn)
{
	int ref = LUA_REFNIL;
	RefMap::iterator it = gRefMap.find(fn);
	if (it != gRefMap.end())
		ref = it->second;

	if (ref != LUA_NOREF && ref != LUA_REFNIL)
	{
		lua_rawgeti(L_, LUA_REGISTRYINDEX, ref);
		if (!lua_isnil(L_, -1))
			return 0;
	}
	
	return 1;
}


LuaSvr* LuaSvr::get()
{
	return luaSvrSelf_;
}


LuaSvr::LuaSvr()
{
	luaMemMax_ = 0;
	luaSvrSelf_ = this;
	timeElapse_ = 0;
	timeGc_ = 0;
	checkthread_ = 1;
}


LuaSvr::LuaSvr(int checkthread)
{
	luaMemMax_ = 0;
	luaSvrSelf_ = this;
	timeElapse_ = 0;
	timeGc_ = 0;
	checkthread_ = checkthread;
}

LuaSvr::~LuaSvr()
{
	call("appClose", "");

	if (scriptCTT_)
	{
		scriptCTT_->terminate();
		SAFE_DELETE(scriptCTT_);
	}
	if (L_)
	{
		lua_close(L_);
		L_ = NULL;
	}
}


void LuaSvr::set(const char *key, const char *val)
{
	lua_pushstring(L_, val);
	lua_setglobal(L_, key);
}


void LuaSvr::init()
{
	L_ = luaL_newstate();

	luaL_openlibs(L_);
	lua_settop(L_, 0);

	//����һ���µ� panic ���ֻţ� ����
	lua_atpanic(L_, error_hook);
	lua_pushcfunction(L_,error_hook);
	stackErrorHook_= lua_gettop(L_);
	
	//ע��c��ӿڸ�lua�ű���
	luaL_register(L_, "_G", LuaGlobal::functions);
	Lua::Lunar<Bit>::Register(L_);
	Lua::Lunar<Timer>::Register(L_);
	Lua::Lunar<LuaNetwork>::Register(L_);
	Lua::Lunar<Connection>::Register(L_);
	lua_pop(L_, 1);

	if (checkthread_ && !scriptCTT_)
	{
		scriptCTT_ = new ScriptTimeCheckThread(L_);
		LOG("init ScriptTimeCheckThread succ!!!!");
	}

	onInit();

#ifndef WIN32
	unsigned long noblock = 1;
	ioctl(STDIN_FILENO, FIONBIO, &noblock);
#endif

}


void LuaSvr::reload()
{
	lua_newtable(L_);
	lua_setfield(L_, LUA_REGISTRYINDEX, "_LOADED");
	luaL_openlibs(L_);

	scriptInit();
}


bool LuaSvr::scriptInit()
{
	const char *mainscript = Config::GetValue("MainScript");

	if (!mainscript)
	{
		ERROR("Can't find main script key section\n");
		return false;
	}
	//for(int i=0;i<strlen(mainscript);++i)
	//	printf("===%c", mainscript[i]);
	INFO("load main script file:%s", mainscript);
	if (luaL_loadfile(L_, mainscript) || lua_pcall(L_, 0, LUA_MULTRET, stackErrorHook_))
	{
		ERROR("err=%s\n", lua_tostring(L_, -1));
		return false;
	}

	initRef();

	const char *init_func = Config::GetValue("InitFunc");
	if (init_func)
	{
		lua_getglobal(L_, init_func);
		return scriptCall(L_, 0, 0);
	}
	else
	{
		lua_getglobal(L_, "init");
		return scriptCall(L_, 0, 0);
	}

}


static bool script_update = false;

void LuaSvr::setScriptUpdate()
{
	script_update = true;
}

void LuaSvr::loadScript()
{
	if (!script_update)
		return;

	if (!L_)
	{
		LOG("lua state is not init!");
		return ;
	}

	script_update = false;
	char scriptPath[128] = "update.lua";
	LOG("load main script file:%s", scriptPath);

	if (LuaSvr::scriptCTT_)
		LuaSvr::scriptCTT_->enter();

	if (luaL_loadfile(L_, scriptPath) || lua_pcall(L_, 0, LUA_MULTRET, stackErrorHook_))
	{
		ERROR("err=%s\n", lua_tostring(L_, -1));
	}
	if (LuaSvr::scriptCTT_)
		LuaSvr::scriptCTT_->leave();
	return;
}

void LuaSvr::run()
{
	bool ret = scriptInit();
	if(!ret)
	{
		ERROR("app init failed");
	}
}

//���������ռ�����
//���������������� what �����ֲ�ͬ������
//LUA_GCSTOP: ֹͣ�����ռ�����
//LUA_GCRESTART: ���������ռ�����
//LUA_GCCOLLECT: ����һ�������������ռ�ѭ����
//LUA_GCCOUNT: ���� Lua ʹ�õ��ڴ��������� K �ֽ�Ϊ��λ����
//LUA_GCCOUNTB: ���ص�ǰ�ڴ�ʹ�������� 1024 ��������
//LUA_GCSTEP: ����һ�����������ռ��������� data ���ƣ�Խ���ֵ��ζ��Խ�ಽ����������庬�壨�������ֱ�ʾ�˶��٣���δ��׼����
//�����������������������ʵ���ԵĲ��� data ��ֵ�������һ��������һ�������ռ����ڣ����ط��� 1 ��
//LUA_GCSETPAUSE: �� data/100 ����Ϊ garbage-collector pause ����ֵ���μ� ��2.10��������������ǰ��ֵ��
//LUA_GCSETSTEPMUL: �� arg/100 ���ó� step multiplier ���μ� ��2.10��������������ǰ��ֵ��
int LuaSvr::mem() const
{
	int mem = lua_gc(L_, LUA_GCCOUNT, 0);
	return mem;
}

void LuaSvr::doUpdate(uint dtime)
{
	//nothing todo
	timeElapse_ += dtime;
	if (timeElapse_ > 100)
	{
		timeElapse_ = 0;
	}

	int memA = lua_gc(L_, LUA_GCCOUNT, 0);
	if (memA > luaMemMax_)
	{
		luaMemMax_ = memA;
	}

	if (lua_gettop(L_) > 10)
		ERROR("lua stack count;%d", lua_gettop(L_));

	timeGc_ += dtime;
	if (timeGc_ > 10000)//10s
	{
		timeGc_ = 0;
		if (lua_gc(L_, LUA_GCSTEP, 256) == 1)
		{
			lua_gc(L_, LUA_GCRESTART, 0);
			LOG("[GC]|memory, luamem=%dM", (lua_gc(L_, LUA_GCCOUNT, 0)) >> 10);
		}
	}

	loadScript();

}

bool LuaSvr::call(const char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	bool ret = call(fmt, va);
	va_end(va);
	return ret;
}

bool LuaSvr::call(const char* fmt, va_list va)
{
	lua_State *L = LuaSvr::get()->L();
	int p = 0, pc = 0;

	char c = 0;

	while((c=fmt[p++]))
	{
		switch(c)
		{
		case 'i':
			{
				int i = va_arg(va, int);
				lua_pushinteger(L, i);
				pc++;
			}
			break;
		case 'l':
			{
				int64 i = va_arg(va, int64);
				lua_pushinteger(L, (int64)i);
				//lua_pushinteger(L, (lua_Number)i);
				pc++;
			}
			break;
		case 'm':
			{
				Buf *buf = va_arg(va, Buf*);
				if (buf)
				{
					int size = buf->getLength();
					LocalBuf data(size);
					buf->read(data, size);
					lua_pushlstring(L, data, size);
				}
				else
				{
					lua_pushlstring(L, "", 0);
				}
				pc++;
			}
			break;
		case 't':
			{
				int i = va_arg(va, int);
				lua_pushvalue(L, i);
				pc++;
			}
			break;
		case 'a':
			{
				//û��
				pc++;
			}
			break;
		case 'S':
			{
				std::string *s = va_arg(va, std::string*);
				if (s)
				{
					lua_pushlstring(L, s->data(), s->size());
				}
				else
				{
					lua_pushnil(L);
				}
				pc++;
			}
			break;
		case '|':
			{
				pc++;
				break;
			}
		default:
			ERROR("undefined argument typed specified:%c, fmt:%s ", c , fmt);
			return false;
		}
	}
	
	return LuaSvr::scriptCall(L, pc, 0);
}

bool LuaSvr::call(const char *fn, const char* fmt, ...)
{
	if (!LuaSvr::get())
	{
		ERROR("LuaSvr pointer miss");
		return false;
	}

	lua_State *L = LuaSvr::get()->L();

	if (LuaSvr::get()->getRef(fn) != 0)
	{
		lua_getglobal(L, fn);
		if (lua_isnil(L, -1))
		{
			lua_pop(L, 1);
			ERROR("Can't find %s to call", fn);
			return false;
		}
	}

	if (!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		ERROR("Can't find %s to call", fn);
		return false;
	}

	va_list va;
	va_start(va, fmt);
	bool ret = call(fmt, va);
	va_end(va);
	return ret;
}


bool LuaSvr::call(const char* fn, int nargs, int nrets)
{
	if (!LuaSvr::get())
	{
		ERROR("LuaSvr pointer miss");
		return false;
	}

	lua_State *L = LuaSvr::get()->L();

	if (LuaSvr::get()->getRef(fn) != 0)
	{
		lua_getglobal(L, fn);
		if (lua_isnil(L, -1))
		{
			lua_pop(L, 1);
			ERROR("Can't find %s to call", fn);
			return false;
		}
	}

	if (!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		ERROR("Can't find %s to call", fn);
		return false;
	}

	return LuaSvr::scriptCall(L, nargs, nrets);
}

bool LuaSvr::scriptCall(lua_State *L, int nargs, int nrets)
{
	bool ret = true;
	int base = lua_gettop(L) - nargs;
	ASSERT(base>0);

	lua_pushcfunction(L, error_hook);
	lua_insert(L, base);
	if (LuaSvr::scriptCTT_)
		LuaSvr::scriptCTT_->enter();

	if (lua_pcall(L, nargs, nrets, base))
	{
		lua_pop(L, 1);
		ret = false;
	}
	lua_remove(L, base);

	if (LuaSvr::scriptCTT_)
		LuaSvr::scriptCTT_->leave();

	return ret;
}


