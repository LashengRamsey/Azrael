#include <string.h>
#include <assert.h>
#include "lunar.h"
#include "celltype.h"
#include "arch.h"
#include "log.h"


namespace Lua
{
	void argParse(lua_State* L, const char* fmt, ...)
	{
		unsigned int n = 0;
		unsigned int pn = 0;
		unsigned int tn = strlen(fmt);

		for(; n<tn; ++n)
		{
			if ( fmt[n] == '|')
				break;
			if ( fmt[n] == '#')
				continue;
			pn++;
		}
		//n is fixed argument count
		if ((int)pn > lua_gettop(L))
		{
			luaL_error(L, "function expected %d argument at least", n);
		}
		va_list va;
		int p = 0;
		int lp = 1;

		va_start(va, fmt);
		char c = 0;

		while((c=fmt[p++]))
		{
			if( lua_gettop(L) < lp )
				break;
			switch(c)
			{
			case 'i':
				{
					int* i = va_arg(va, int*);
					luaL_checktype(L, lp, LUA_TNUMBER);
					*i = (int)lua_tointeger(L, lp++);
				}
				break;
			case 'l':
				{
					LUA_INTEGER* i = va_arg(va, LUA_INTEGER*);
					//int64* i = va_arg(va, int64*);
					luaL_checktype(L, lp, LUA_TNUMBER);
					*i = lua_tointeger(L, lp++);
				}
				break;
			case 'f':
			case 'n':
				{
					float* f = va_arg(va, float*);
					luaL_checktype(L, lp, LUA_TNUMBER);
					*f = (float)lua_tonumber(L, lp++);
				}
				break;
			case 's':
				{
					char** s = va_arg(va, char**);
					luaL_checktype(L, lp, LUA_TSTRING);
					size_t len = 0;
					*s = (char*)luaL_checklstring(L, lp++, &len);

					if(fmt[p] == '#')
					{
						unsigned int* i = va_arg(va, unsigned int*);
						*i = len;
						p++;
					}
					break;
				}
				break;
			case 'u':
				{
					void** u = va_arg(va, void**);
					luaL_checktype(L, lp, LUA_TUSERDATA);
					*u = (char*)lua_touserdata(L, lp++);
				}
				break;
			case 'p':
				{
					void** u = va_arg(va, void**);
					luaL_checktype(L, lp, LUA_TLIGHTUSERDATA);
					*u = (void*)lua_topointer(L, lp++);
				}
				break;

			case 'b':
				{
					bool* b = va_arg(va, bool*);
					luaL_checktype(L, lp, LUA_TBOOLEAN);
					*b = lua_toboolean(L, lp++) == 1;
				}
				break;
			case 'r':
				{
					lua_pushvalue(L, lp++);
					int* ref = va_arg(va, int*);
					*ref = luaL_ref(L, LUA_REGISTRYINDEX);
				}
				break;
			case 't':
				{
					//lua的table在c层表现为指针地址
					luaL_checktype(L, lp, LUA_TTABLE);
					int* ref = va_arg(va, int*);
					*ref = lp++;
				}
				break;

			case '|':
				break;
			case '_':
				lp++;
				break;
			default:
				ERRLOG("undefined argument typed specified, %d", c);
				break;
			}
		}
		va_end(va);
	}


	int returnValue(lua_State* L, const char* fmt, ...)
	{
		va_list va;
		int p=0,pc=0;

		va_start(va, fmt);
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
					lua_pushinteger(L, (lua_Integer)i);
					pc++;
				}
				break;
			
			case 'b':
				{
					bool b = va_arg(va, int) > 0;
					lua_pushboolean(L, (int)b);
					pc++;
				}
				break;
			
			case 's':
				{
					char* s = va_arg(va, char*);
					lua_pushstring(L, s);
					pc++;
				}
				break;
			case 'f':
			case 'n':
				{
					double d = va_arg(va, double);
					lua_pushnumber(L, d);
					pc++;
				}
				break;
			default:
				ERRLOG("undefined argument typed specified,%d", c);
				break;
			}
		}
		va_end(va);
		return pc;
	}


}