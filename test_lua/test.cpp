#include <string>
#include "test.h"


static lua_State* gLuaState;

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
	//���ýű�
	//LuaSvr::call("CHandlerError", "S", &errdump);

	return 0;
}

bool scriptCall(lua_State *L, int nargs, int nrets)
{
	bool ret = true;
	//ȷ��������
	int base = lua_gettop(L) - nargs;
	//nargs=2
	//function args1 args2
	//base = 1
	stackDump(gLuaState);
	lua_pushcfunction(L, error_hook);
	stackDump(gLuaState);
	//function args1 args2 error_hook
	lua_insert(L, base);//��ջ��Ԫ�ز���ָ������Ч���������������ƶ��������֮�ϵ�Ԫ��
						//error_hook function args1 args2 args3 
	stackDump(gLuaState);
	if (lua_pcall(L, nargs, nrets, base))
	{
		stackDump(gLuaState);
		lua_pop(L, 1);
		ret = false;
	}
	stackDump(gLuaState);
	lua_remove(L, base);	//��error_hook�Ӷ�ջ���Ƴ�
	stackDump(gLuaState);
	return ret;
}



void init_test()
{	
	if (gLuaState)
		return;
	gLuaState = luaL_newstate();
	luaL_openlibs(gLuaState);//��������lua��׼��
	lua_settop(gLuaState, 0);//��ն�ջ
}

void test_load_lua()
{
	lua_settop(gLuaState, 0);//��ն�ջ
	//nresults==LUA_MULTRET�����еķ���ֵ����push��ջ
	//nresults != LUA_MULTRET������ֵ��������nresults������
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
	lua_settop(gLuaState, 0);//��ն�ջ
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


void test_loadConfig(const char *file)
{
	lua_settop(gLuaState, 0);//��ն�ջ
	printf("====test_loadConfig====\n");
	//lua_State *L = luaL_newstate();
	//luaL_openlibs(L);
	std::string path(file);
	size_t pos = path.find_last_of("/");

	if (pos != std::string::npos)
	{
		path = path.substr(0, pos);
		lua_pushstring(gLuaState, path.c_str());
		lua_setglobal(gLuaState, "ConfigPath");
	}

	if (luaL_dofile(gLuaState, file))
	{
		//ERRLOG("%s", lua_tostring(gLuaState, -1));
		return ;
	}

	int idxG;
	char *key = NULL;
	char *val = NULL;
	lua_getglobal(gLuaState, "Config");
	if (!lua_istable(gLuaState, -1))
		return ;

	idxG = lua_gettop(gLuaState);
	lua_pushnil(gLuaState);
	stackDump(gLuaState);
	//lua_next�Ȱ� ��(luaջ index��ָ�ı�), �ĵ�ǰ�����������ٰ�table ��ǰ������ֵ����
	//�����ص�˵��һ��lua_next����ִ�в����������ģ����ж���һ��key��ֵ
	//�����ֵ����ջ���������nil�����ʾ��ǰȡ������table�е�һ��Ԫ�ص�ֵ����
	//Ȼ�������ǰ��key����ʱ�Ȱ�ջ����ջ������key��ջ�������key��Ӧ��ֵ��ջ��
	//����ջ������table�е�һ����������Ԫ�ص�ֵ��
	//�������ֵ������Ҫ�����ֵ��ջ����key��ջ���Ա������������������һ��keyֵ�㲻����һ��keyֵʱ��lua_next����0������ѭ����


	while (lua_next(gLuaState, idxG) != 0)
	{
		stackDump(gLuaState);
		key = (char*)lua_tostring(gLuaState, -2);
		val = (char*)lua_tostring(gLuaState, -1);
		//setenv(key, val ? val : "", 1);
		lua_pop(gLuaState, 1);
		stackDump(gLuaState);
	}
	//lua_close(L);
	//return 0;
}