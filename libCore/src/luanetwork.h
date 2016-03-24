#ifndef LUANETWORK_H
#define LUANETWORK_H

#include "lunar.h"

class LuaNetwork
{
public:
	LUA_EXPORT(LuaNetwork)

	static int ConnectRouter(lua_State* L);
	static int ConnectDB(lua_State* L);
	static int GetSessionIP(lua_State* L);
	static int SetSessionUserData(lua_State* L);
	static int CloseSession(lua_State* L);
	static int SendToServer(lua_State* L);
	static int SendToNet(lua_State* L);
	static int SendToDB(lua_State* L);
	static int SendToGameServer(lua_State* L);
	static void resolvePacketTableItem(lua_State* L, class Buf* buf);
};


#endif //!LUANETWORK_H