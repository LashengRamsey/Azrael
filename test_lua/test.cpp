#include <string>
#include "test.h"


static lua_State* gLuaState;

//dump整个堆栈的内容
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

static int error_hook(lua_State *L)
{
	//ERRLOG("[LUA ERROR] %s", lua_tostring(L, -1));
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
	//ERRLOG(errdump.c_str());
	//ERRLOG("=======Report error to server========");

	printf("%s\n", errdump.c_str());
	//调用脚本
	//LuaSvr::call("CHandlerError", "S", &errdump);

	return 0;
}

bool scriptCall(lua_State *L, int nargs, int nrets)
{
	bool ret = true;
	//确保参数够
	int base = lua_gettop(L) - nargs;
	//nargs=2
	//function args1 args2
	//base = 1
	stackDump(gLuaState);
	lua_pushcfunction(L, error_hook);
	stackDump(gLuaState);
	//function args1 args2 error_hook
	lua_insert(L, base);//把栈顶元素插入指定的有效索引处，并依次移动这个索引之上的元素
						//error_hook function args1 args2 args3 
	stackDump(gLuaState);
	if (lua_pcall(L, nargs, nrets, base))
	{
		stackDump(gLuaState);
		lua_pop(L, 1);
		ret = false;
	}
	stackDump(gLuaState);
	lua_remove(L, base);	//把error_hook从堆栈中移除
	stackDump(gLuaState);
	return ret;
}



void init_test()
{	
	if (gLuaState)
		return;
	gLuaState = luaL_newstate();
	luaL_openlibs(gLuaState);//载入所有lua标准库
	lua_settop(gLuaState, 0);//清空堆栈
}

void test_load_lua()
{
	//nresults==LUA_MULTRET，所有的返回值都会push进栈
	//nresults != LUA_MULTRET，返回值个数根据nresults来调整
	if (luaL_loadfile(gLuaState, "test.lua") || lua_pcall(gLuaState, 0, LUA_MULTRET, 0))
	{
		printf("test_load_lua luaL_loadfile error\n");
		return;
	}
	lua_getglobal(gLuaState, "test_func");
	lua_pushnumber(gLuaState, 5);
	lua_pushnumber(gLuaState, 6);
	
	scriptCall(gLuaState, 2, 1);
}

void test_lua_insert()
{
	lua_settop(gLuaState, 0);//清空堆栈
	for (int i = 1; i <= 5; ++i)
		lua_pushnumber(gLuaState, i);
	printf("before lua_insert\n");
	//1 2 3 4 5
	stackDump(gLuaState);
	lua_insert(gLuaState, 2);
	//5 1 2 3 4
	printf("after lua_insert\n");
	stackDump(gLuaState);
}
