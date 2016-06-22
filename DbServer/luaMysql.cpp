#include <lua.hpp>
#include <string.h>
#include <stdlib.h>
#ifdef WIN32
#include <Windows.h>
#endif

#include "mysql.h"
#include "luaMysql.h"
#include <vector>
#include <string>


LUA_IMPLE(LuaMysql, LuaMysql);

LUA_FUNC(LuaMysql)
L_METHOD(LuaMysql, c_mysql_init)
L_METHOD(LuaMysql, c_mysql_connect)
L_METHOD(LuaMysql, c_mysql_disconntect)
L_METHOD(LuaMysql, c_mysql_query)
L_METHOD(LuaMysql, c_mysql_insert)
L_METHOD(LuaMysql, c_mysql_update)
L_METHOD(LuaMysql, c_mysql_character_set)
LUA_FUNC_END


int LuaMysql::c_mysql_init(lua_State* L)
{
	MYSQL *mysql = mysql_init(NULL);
	if (!mysql)
	{
		return luaL_error(L, "init mysql error");
	}

	mysql_set_character_set(mysql, "utf8");
	lua_pushlightuserdata(L, mysql);
	return 1;
}

int LuaMysql::c_mysql_connect(lua_State* L)
{
	if (lua_gettop(L) != 5)
		return luaL_error(L, "argument error");

	MYSQL *mysql = mysql_init(NULL);
	if (!mysql)
	{
		return luaL_error(L, "init mysql error");
	}

	//set auto reconnect
	char value = 1;
	mysql_options(mysql, MYSQL_OPT_RECONNECT, &value);
	const char *db_addr = lua_tostring(L, 1);
	const char *db_user = lua_tostring(L, 2);
	const char *db_pass = lua_tostring(L, 3);
	const char *db_name = lua_tostring(L, 4);
	unsigned int db_port = (unsigned int)lua_tonumber(L, 5);

	//connect mysql
	if (!mysql_real_connect(mysql, db_addr, db_user, db_pass, db_name, db_port, NULL, 0))
	{
		return luaL_error(L, "failed to connect database, mysql_err:%s\n", mysql_error(mysql));
	}

	mysql_set_character_set(mysql, "utf8");
	lua_pushlightuserdata(L, mysql);
	return 1;
}

int LuaMysql::c_mysql_disconntect(lua_State* L)
{
	if (lua_gettop(L) > 0)
	{
		MYSQL *mysql = (MYSQL*)lua_touserdata(L, 1);
		mysql_close(mysql);
	}
	return 0;
}

typedef struct stfield
{
	std::string name;
	int type;
}stfield;
//return double dimesional lua table
int LuaMysql::c_mysql_query(lua_State* L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushnumber(L, -1);
		lua_pushnil(L);
		return 2;
	}

	MYSQL *mysql = (MYSQL*)lua_touserdata(L, 1);
	size_t len = 0;
	const char* query_str = lua_tolstring(L, 2, &len);
	if (mysql_ping(mysql) != 0)
	{
		lua_pushnumber(L, -2);
		lua_pushnil(L);
		return 2;
	}

	if (mysql_real_query(mysql, query_str, len))
	{
		lua_pushnumber(L, -3);
		lua_pushnil(L);
		printf("mysql query error:%s,mysql_err:%s\n", query_str, mysql_error(mysql));
		return 2;
	}

	//result of query
	MYSQL_RES *res = mysql_store_result(mysql);
	//fieldcount
	int filednum = mysql_num_fields(res);
	std::vector<stfield> vec_fields;
	MYSQL_FIELD *field;
	while((field = mysql_fetch_field(res)))
	{
		struct stfield f;
		f.name = field->name;
		f.type = field->type;

		vec_fields.push_back(f);
	}

	MYSQL_ROW row;
	int rowno = 1;
	//set result to lua table
	lua_pushnumber(L, 0);
	lua_newtable(L);
	while((row = mysql_fetch_row(res)))
	{
		lua_pushnumber(L, rowno++);
		lua_newtable(L);
		int i = 0;

		for (i = 0; i< filednum; ++i)
		{
			lua_pushstring(L, vec_fields[i].name.c_str());

			switch(vec_fields[i].type)
			{
			case MYSQL_TYPE_TINY:
			case MYSQL_TYPE_SHORT:
			case MYSQL_TYPE_INT24:
			case MYSQL_TYPE_LONG:
			case MYSQL_TYPE_LONGLONG:
				if (row[i])
				{
#ifdef WIN32
					lua_pushnumber(L, _atoi64(row[i]));
#else
					lua_pushnumber(L, atoill(row[i]));
#endif
				}
				else
				{
					lua_pushnumber(L, 0);
				}
				break;

			default:
				lua_pushstring(L, row[i]);
				break;
			}
			lua_settable(L, -3);
		}
		lua_settable(L, -3);
	}
	//free result
	mysql_free_result(res);
	return 2;
}

int LuaMysql::c_mysql_insert(lua_State* L)
{
	//return inset_id
	if (lua_gettop(L) != 2)
	{
		lua_pushnumber(L, -1);
		lua_pushnil(L);
		return 2;
	}

	MYSQL *mysql = (MYSQL*)lua_touserdata(L, 1);
	size_t len = 0;
	const char* insert_str = lua_tolstring(L, 2, &len);
	if (mysql_ping(mysql) != 0)
	{
		lua_pushnumber(L, -2);
		lua_pushnil(L);
		return 2;
	}

	//insert mysql
	int result = mysql_real_query(mysql, insert_str, len);
	if (result)
	{
		lua_pushnumber(L, -3);
		lua_pushnil(L);
		printf("mysql insert error:%s,mysql_err:%s\n", insert_str, mysql_error(mysql));
		return 2;
	}

	if (mysql_store_result(mysql) == NULL &&
		mysql_field_count(mysql) == 0 &&
		mysql_insert_id(mysql) != 0)
	{
		unsigned long long insert_id = mysql_insert_id(mysql);
		lua_pushnumber(L, 0);
		lua_pushnumber(L, insert_id);
	}
	else
	{
		lua_pushnumber(L, 0);
		lua_pushnil(L);
	}
	return 2;

}

int LuaMysql::c_mysql_update(lua_State* L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushnumber(L, -1);
		return 1;
	}

	MYSQL *mysql = (MYSQL*)lua_touserdata(L, 1);
	size_t len = 0;
	const char* update_str = lua_tolstring(L, 2, &len);
	if (mysql_ping(mysql) != 0)
	{
		lua_pushnumber(L, -2);
		return 1;
	}

	//update mysql
	int result = mysql_real_query(mysql, update_str, len);
	if (result)
	{
		lua_pushnumber(L, -3);
		printf("mysql update error:%s,mysql_err:%s\n", update_str, mysql_error(mysql));
		return 1;
	}

	lua_pushnumber(L, 0);
	return 1;
}

int LuaMysql::c_mysql_character_set(lua_State* L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushnumber(L, -1);
		return 1;
	}

	MYSQL *mysql = (MYSQL*)lua_touserdata(L, 1);
	const char *character_type = lua_tostring(L, 2);
	mysql_set_character_set(mysql, character_type);
	lua_pushnumber(L, 0);
	return 1;
}











