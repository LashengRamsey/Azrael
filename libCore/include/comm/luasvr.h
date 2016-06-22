#ifndef LUASVR_H
#define LUASVR_H

#include "lua.hpp"
#include "celltype.h"
#include "log.h"

enum LuaRef{
	REF_IDLE = 1,
	REF_DO_TIMER,
	REF_DO_MSG,
	REF_CONNECT,
	REF_DISCONNECT,
	REF_ERROR,
	REF_NET_MSG,
	REF_MAX,
};

struct LuaStackKeeper
{
	LuaStackKeeper(lua_State* L)
	{
		L_=L;
		top = lua_gettop(L);
	}
	~LuaStackKeeper()
	{
		lua_settop(L_, top);
	}

	lua_State* L_;
	int top;
};


//·â×°lua_State
class LuaSvr
{
public:
	LuaSvr();
	LuaSvr(int checkthread);
	virtual ~LuaSvr();

	lua_State* L()
	{
		ASSERT(L_);
		return L_;
	}

	void set(const char* key, const char* val);
	void init();
	void reload();
	virtual void doUpdate(uint dtime);
	virtual void onInit(){}

	void run();
	int mem() const;
	int memMax() const { return luaMemMax_; }
	static LuaSvr* get();
	static void release();

	static bool call(const char* z, const char* fmt, ...);
	static bool call(const char* fmt, ...);
	static bool call(const char* fmt, va_list va);
	static bool call(const char* fname, int nargs, int nrets);

	int getRef(int ref);
	int getRef(const char* fn);
	static bool scriptCall(lua_State* L, int nargs, int nrets);
	void setScriptUpdate();
	void loadScript();

protected:
	virtual void initRef();

private:
	bool scriptInit();

	static class ScriptTimeCheckThread* scriptCTT_;

	static LuaSvr* luaSvrSelf_;
	lua_State* L_;

	int stackErrorHook_;
	int stackMsgHandler_;
	int luaMemMax_;

	uint timeElapse_;
	uint timeGc_;
	uint checkthread_;

};


#endif //LUASVR_H
