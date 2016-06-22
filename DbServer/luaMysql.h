#ifndef LUAMYSQL_H
#define LUAMYSQL_H

#include "lunar.h"

class LuaMysql
{
public:
	LUA_EXPORT(LuaMysql)

	static int c_mysql_init(lua_State* L);
	static int c_mysql_connect(lua_State* L);
	static int c_mysql_disconntect(lua_State* L);
	static int c_mysql_query(lua_State* L);
	static int c_mysql_insert(lua_State* L);
	static int c_mysql_update(lua_State* L);
	static int c_mysql_character_set(lua_State* L);
};



#endif //!LUAMYSQL_H
