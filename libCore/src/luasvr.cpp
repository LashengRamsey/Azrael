#include <string.h>
#include <fcntl.h>
#include "luasvr.h"
#include "net.h"
#include "app.h"
#include "luaglobal.h"
#include "luabit.h"
#include "timer.h"
#include "connection.h"
#include "luanetwork.h"
#include "lua_module_register.h"

//#define LuaDebug

LuaSvr* LuaSvr::luaSvrSelf_ = NULL;
static int gRef[REF_MAX];
typedef std::map<std::string, int> FuncNameRefMap;
FuncNameRefMap gFuncNameRefMap;//�����ַ�����Ӧ��refֵ
//�ű����ִ��ʱ��
uint ScriptTimeCheckThread::maxScriptTime = 10000;
ScriptTimeCheckThread* LuaSvr::scriptCTT_ = NULL;
static bool script_update = false;

//dump������ջ������
static void stackDump(lua_State *L) {
	int i;
	int top = lua_gettop(L);
	for (i = 1; i <= top; i++) {  /* repeat for each level */
		int t = lua_type(L, i);
		switch (t) {

		case LUA_TSTRING:  /* strings */
			printf("`%s'", lua_tostring(L, i));
			break;

		case LUA_TBOOLEAN:  /* booleans */
			printf(lua_toboolean(L, i) ? "true" : "false");
			break;

		case LUA_TNUMBER:  /* numbers */
			printf("%g", lua_tonumber(L, i));
			break;

		default:  /* other values */
			printf("%s", lua_typename(L, t));
			break;

		}
		printf("  ");  /* put a separator */
	}
	printf("\n");     /* end the listing */
}

//�ű�������Ϣ
//panic �������Դ�ջ��ȡ��������Ϣ
static int error_hook(lua_State *L)
{
	ERRLOG("[LUA ERROR] %s", lua_tostring(L, -1));
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
		snprintf(errline, 8096, "[LUA ERRLOG] %s '%s' @'%s:%d'\n",
			ldb.what, name, filename, ldb.currentline);
		errdump += errline;
	}
	ERRLOG(errdump.c_str());
	ERRLOG("=======Report error to server========");

	printf("%s\n", errdump.c_str());
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
void setRef(lua_State *L, const char *funcName, int ref)
{
	//luaL_ref��ջ�е���һ��ֵ����һ���µ�������Ϊkey���䱣�浽registry�У����������key
	gRef[ref] = luaL_ref(L, LUA_REGISTRYINDEX);
	gFuncNameRefMap[funcName] = gRef[ref];
}

ScriptTimeCheckThread::ScriptTimeCheckThread(lua_State *L) : Thread(false)
{
	L_ = L;
	enterTimer_ = 0;
	level_ = 0;
	break_ = 0;
	run();
}

void ScriptTimeCheckThread::enter()
{
	if (level_ == 0) {
		enterTimer_ = (uint)time(NULL);
	}
	level_++;
}

void ScriptTimeCheckThread::leave()
{
	level_--;
	if (level_ == 0) {
		enterTimer_ = 0;
	}
}

void ScriptTimeCheckThread::timeoutBreak(lua_State *L, lua_Debug *D)
{
	lua_sethook(L, NULL, 0, 0);
	luaL_error(L, "Script timeout over 10 seconds.");
}

void ScriptTimeCheckThread::work()
{
	int enterTime = 0;
	while (true)
	{
		thread_sleep(1000);
		enterTime = enterTimer_;
		if (enterTime && maxScriptTime && time(NULL) - enterTime > maxScriptTime)
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

//��ȡ�ű�����
void LuaSvr::initRef()
{
	for (int i = 0; i < REF_MAX; ++i)
	{
		gRef[i] = LUA_NOREF;
	}

	lua_getglobal(L_, "CHandlerTimer");
	if (!lua_isfunction(L_, -1))//if (lua_isnil(L_, -1))
	{
		FATAL("[LUA FATAL] lua script no CHandlerTimer function****");
	}
	setRef(L_, "CHandlerTimer", REF_DO_TIMER);

	lua_getglobal(L_, "CHandlerMsg");
	if (!lua_isfunction(L_, -1))//if (lua_isnil(L_, -1))
	{
		FATAL("[LUA FATAL] lua script no CHandlerMsg function****");
	}
	setRef(L_, "CHandlerMsg", REF_DO_MSG);

	lua_getglobal(L_, "CHandlerConnect");
	if (!lua_isfunction(L_, -1))//if (lua_isnil(L_, -1))
	{
		FATAL("[LUA FATAL] lua script no CHandlerConnect function****");
	}
	setRef(L_, "CHandlerConnect", REF_CONNECT);

	lua_getglobal(L_, "CHandlerDisconnect");
	if (!lua_isfunction(L_, -1))//if (lua_isnil(L_, -1))
	{
		FATAL("[LUA FATAL] lua script no CHandlerDisconnect function****");
	}
	setRef(L_, "CHandlerDisconnect", REF_DISCONNECT);

	lua_getglobal(L_, "CHandlerError");
	if (!lua_isfunction(L_, -1))//if (lua_isnil(L_, -1))
	{
		FATAL("[LUA FATAL] lua script no CHandlerError function****");
	}
	setRef(L_, "CHandlerError", REF_ERROR);

	lua_getglobal(L_, "CHandlerNetMsg");
	if (!lua_isfunction(L_, -1))//if (lua_isnil(L_, -1))
	{
		FATAL("[LUA FATAL] lua script no CHandlerNetMsg function****");
	}
	setRef(L_, "CHandlerNetMsg", REF_NET_MSG);
}

//����fn���Ѷ�Ӧ�ĺ�����ջ
int LuaSvr::getRef(int ref)
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

//����fn���Ѷ�Ӧ�ĺ�����ջ
int LuaSvr::getRef(const char* funcName)
{
	int ref = LUA_REFNIL;
	FuncNameRefMap::iterator it = gFuncNameRefMap.find(funcName);
	if (it != gFuncNameRefMap.end()) {
		ref = it->second;
	}

	if (ref != LUA_NOREF && ref != LUA_REFNIL)
	{	
		//��Ӧ�ĺ�����ջ
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

//����ȫ�ֱ���
void LuaSvr::set(const char *key, const char *val)
{
	lua_pushstring(L_, val);
	lua_setglobal(L_, key);
}

void LuaSvr::init()
{
	L_ = luaL_newstate();//����һ�������
	if (!L_) {
		FATAL("[LUA FATAL] LuaSvr init luaL_newstate error");
		return;
	}
	luaL_openlibs(L_);//��������lua��׼��
	lua_settop(L_, 0);//��ն�ջ

	//����һ���µ� panic����
	lua_atpanic(L_, error_hook);
	lua_pushcfunction(L_,error_hook);
	stackErrorHook_= lua_gettop(L_);
	
	//ע��c��ӿڸ�lua�ű���,��luaL_Reg �����е����к���ע�ᵽlua��
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
	luaopen_struct(L_);
	luaopen_pb(L_);
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

void LuaSvr::run()
{
	bool ret = scriptInit();
	if (!ret)
	{
		ERRLOG("luasvr run failed");
		printf("luasvr run failed\n");
	}
}

bool LuaSvr::scriptInit()
{
	//��ʼ���ű�
	const char *mainscript = Config::GetValue("MainScript");
	if (!mainscript)
	{
		ERRLOG("Can't find main script key section\n");
		return false;
	}
	
	INFO("load main script file:%s", mainscript);
	if (luaL_loadfile(L_, mainscript) || lua_pcall(L_, 0, LUA_MULTRET, stackErrorHook_))
	{
		ERRLOG("err=%s\n", lua_tostring(L_, -1));
		return false;
	}

	initRef();
	//��ʼ�ű����ú�����Ĭ����init
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

void LuaSvr::setScriptUpdate()
{
	script_update = true;
}

//���½ű�
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
		ERRLOG("err=%s\n", lua_tostring(L_, -1));
	}
	if (LuaSvr::scriptCTT_)
		LuaSvr::scriptCTT_->leave();
	return;
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
		ERRLOG("lua stack count;%d", lua_gettop(L_));

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
				//int64 i = va_arg(va, int64);
				//lua_pushinteger(L, (int64)i);
				//lua_pushinteger(L, (lua_Number)i);
				lua_Integer i = va_arg(va, lua_Integer);
				lua_pushinteger(L, i);
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
			ERRLOG("undefined argument typed specified:%c, fmt:%s ", c , fmt);
			return false;
		}
	}
	
	return LuaSvr::scriptCall(L, pc, 0);
}

bool LuaSvr::call(const char *fn, const char* fmt, ...)
{
	if (!LuaSvr::get())
	{
		ERRLOG("LuaSvr pointer miss");
		return false;
	}

	lua_State *L = LuaSvr::get()->L();

	if (LuaSvr::get()->getRef(fn) != 0)
	{
		lua_getglobal(L, fn);
		if (lua_isnil(L, -1))
		{
			lua_pop(L, 1);
			ERRLOG("Can't find %s to call", fn);
			return false;
		}
	}

	if (!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		ERRLOG("Can't find %s to call", fn);
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
		ERRLOG("LuaSvr pointer miss");
		return false;
	}

	lua_State *L = LuaSvr::get()->L();

	if (LuaSvr::get()->getRef(fn) != 0)
	{
		lua_getglobal(L, fn);
		if (lua_isnil(L, -1))
		{
			lua_pop(L, 1);
			ERRLOG("Can't find %s to call", fn);
			return false;
		}
	}

	if (!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		ERRLOG("Can't find %s to call", fn);
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
