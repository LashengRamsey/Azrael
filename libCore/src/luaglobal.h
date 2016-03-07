#include "lunar.h"

class StringBuilder;
class LuaGlobal
{
public:
	LUA_EXPORT(LuaGlobal)

	//export function
	static int C_TableToStr(lua_State* L);
	static int C_Log(lua_State* L);
	static int C_Info(lua_State* L);
	static int C_Error(lua_State* L);
	static int C_GetServerID(lua_State* L);
	static int C_GetConfig(lua_State* L);
	static int C_GetMTime(lua_State* L);
	static int C_StopServer(lua_State* L);
	static int C_GetHashCode(lua_State* L);
	static int C_ToNumber(lua_State* L);
};
