#include "luaglobal.h"
#include <string.h>
#include "log.h"
#include "app.h"
#include "arch.h"
#include "buf.h"
#include "mqnet.h"
#include "net.h"



LUA_IMPLE(LuaGlobal, LuaGlobal);
LUA_METD(LuaGlobal)
LUA_METD_END
LUA_FUNC(LuaGlobal)
L_METHOD(LuaGlobal, TableToStr)
L_METHOD(LuaGlobal, Log)
L_METHOD(LuaGlobal, Info)
L_METHOD(LuaGlobal, Error)
L_METHOD(LuaGlobal, GetServerID)
L_METHOD(LuaGlobal, GetConfig)
L_METHOD(LuaGlobal, GetMTime)
L_METHOD(LuaGlobal, StopServer)
L_METHOD(LuaGlobal, GetHashCode)
LUA_FUNC_END


class StringBuilder
{
public:
	static const int BUFSIZE = 8192*100;
	StringBuilder(lua_State*L)
	{
		p_=0;
		lvl_=0;
	}

	void addString(const char* v)
	{
		int n=strlen(v);
		addLString(v, n);
	}

	void addLString(const char* v, size_t l)
	{
		if ((int)(p_+l) < BUFSIZE)
		{
			strcpy(buffer+p_, v);
			p_+=l;
		}
		else
			lvl_++;
	}

	void addChar(char c)
	{
		if(p_+1 < BUFSIZE)
		{
			buffer[p_] = c;
			p_++;
		}
		else
			lvl_++;
	}

	StringBuilder& operator<<(const char* v)
	{
		addString(v);
		return *this;
	}
	StringBuilder& operator<<(int v)
	{
		char i[32];
		int n=sprintf(i, "%d", v);
		addLString(i, n);
		return *this;
	}
	StringBuilder& operator<<(int64 v)
	{
		char i[32];
		int n=sprintf(i, "%lld", v);
		addLString(i, n);
		return *this;
	}
	StringBuilder& operator<<(bool v)
	{
		addString(v?"true":"false");
		return *this;
	}

	void addquoted(const char* s, size_t l)
	{
		addChar('"');
		while(l--)
		{
			switch(*s)
			{
			case '"':
			case '\\':
			case '\n':
				{
					addChar('\\');
					addChar(*s);
					break;
				}
			case '\r':
				{
					addLString("\\r", 2);
					break;
				}
			case '\0':
				{
					addLString("\\000", 4);
					break;
				}
			default:
				{
					addChar(*s);
					break;
				}
			}
			s++;
		}
		addChar('"');
	}


	char buffer[BUFSIZE];
	int p_;
	int lvl_;
};


static const char* s_keywords[] = {
	"and","break","do","else","elseif",
	"end","false","for","function","if",
	"in","local","nil","not","or",
	"repeat","return","then","true","until","while", NULL, NULL
};

static bool isKeyword(const char* s, size_t l)
{
	const char *word;
	int i = 0;
	while((word = s_keywords[i++]))
	{
		if (strncmp(word, s, l) == 0)
		{
			return true;
		}
	}
	return false;
}

static bool isToken(const char* s, size_t l)
{
	if (l == 0)
		return false;

	if ( isKeyword(s, l))
		return false;
	if (!isalpha(s[0]))
		return false;

	for(size_t i=1; i < l; ++i)
	{
		if (!isalnum(s[i]) && s[i] != '_')
			return false;
	}
	return true;
}


static void tableToStr(lua_State* L, StringBuilder& stream, int lv, bool writefloat)
{
	if (lv > 12)
	{
		luaL_error(L, "table too many lv to convert to string");
		return ;
	}
	int t = lua_gettop(L);
	lua_pushnil(L); //first key

	int it;
	while(( it = lua_next(L, t)) != 0)
	{
		switch(lua_type(L, -2))
		{
		case LUA_TNUMBER:
			{
				stream << "[" << (int64)lua_tonumber(L, -2) << "]=";
				break;
			}
		case LUA_TSTRING:
			{
				size_t len;
				const char* str = lua_tolstring(L, -2, &len);
				if (!len)
				{
					luaL_error(L, "Can't use empty string as key");
				}
				if (isToken(str, len))
					stream << str << "=";
				else
					stream << "['" << str << "']=";
				break;
			}
		default:
			luaL_error(L, "don't support pickle key type");
			return ;
		}

		switch(lua_type(L, -1))
		{
		case LUA_TNUMBER:
			{
				if (writefloat)
					stream << lua_tostring(L, -1);
				else
					stream << (int64)lua_tonumber(L, -1);
				break;
			}
		case LUA_TSTRING:
			{
				size_t l;
				const char *s = lua_tolstring(L, -1, &l);
				stream.addquoted(s, l);
				break;
			}
		case LUA_TBOOLEAN:
			{
				stream << (bool)lua_toboolean(L, -1);
				break;
			}
		case LUA_TTABLE:
			{
				stream<<"{";
				tableToStr(L, stream, lv+1, writefloat);
				stream<<"}";
				break;
			}
		default:
			luaL_error(L, "don't support pickle value type");
			return;
		}
		stream << ",";
		lua_pop(L, 1);

		if(stream.lvl_>0)
		{
			luaL_error(L, "table too big to convert to string");
			return;
		}
	}
}

//DJB�㷨
static int DJBHash(std::string str)
{
	int hash = 5381;
	for (uint i = 0;i < str.length(); ++i)
	{
		hash = ((hash << 5) + hash) + str.at(i);
	}
	return (hash & 0x7FFFFFFF);
}


int LuaGlobal::Log(lua_State* L)
{
	const char *str = lua_tostring(L, 1);
	LOG("%s", str);
	return 0;
}

int LuaGlobal::Info(lua_State* L)
{
	const char *str = lua_tostring(L, 1);
	INFO("%s", str);
	return 0;
}

int LuaGlobal::Error(lua_State* L)
{
	const char *str = lua_tostring(L, 1);
	ERROR("%s", str);
	return 0;
}

int LuaGlobal::GetServerID(lua_State* L)
{
	return Lua::returnValue(L, "i", ServerApp::get()->getServerID());
}

int LuaGlobal::GetConfig(lua_State* L)
{
	char *key;
	Lua::argParse(L, "s", &key);
	return Lua::returnValue(L, "s", Config::GetValue(key));
}

int LuaGlobal::TableToStr(lua_State* L)
{
	int n = lua_gettop(L);
	if (!n || n>2)
	{
		luaL_error(L, "Function should pass one parameter at least,two parameter at most");
		return 0;
	}

	luaL_checktype(L, -n, LUA_TTABLE);
	bool writefloat = false;
	if (n > 1)
	{
		writefloat = lua_toboolean(L, -1);
		lua_pushvalue(L, -2);
	}

	StringBuilder sstr(L);
	sstr<<"{";
	tableToStr(L, sstr, 0, writefloat);
	sstr<<"}";

	sstr.buffer[sstr.p_] = 0;
	lua_pushlstring(L, sstr.buffer, sstr.p_);
	return 1;
}

int LuaGlobal::GetMTime(lua_State* L)
{
	lua_pushnumber(L, timer_get_time());
	return 1;
}

int LuaGlobal::StopServer(lua_State* L)
{
	ServerApp::get()->stop();
	return 0;
}

int LuaGlobal::GetHashCode(lua_State* L)
{
	int code = 0;
	if (lua_type(L, 1) == LUA_TSTRING)
	{
		const char *str = lua_tostring(L, 1);
		code = DJBHash(str);
	}
	else
	{
		ERROR("GetHashCode expected string,got nil");
	}
	lua_pushnumber(L, code);
	return 1;
}











