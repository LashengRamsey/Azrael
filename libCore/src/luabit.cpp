#include "luabit.h"
#include "celltype.h"

LUA_IMPLE(Bit, Bit);
LUA_METD(Bit)
LUA_METD_END
LUA_FUNC(Bit)
L_METHOD(Bit, c_bnot)
L_METHOD(Bit, c_band)
L_METHOD(Bit, c_band64)
L_METHOD(Bit, c_bor)
L_METHOD(Bit, c_bor64)
L_METHOD(Bit, c_bxor)
L_METHOD(Bit, c_bxor64)
L_METHOD(Bit, c_blsh)
L_METHOD(Bit, c_blsh64)
L_METHOD(Bit, c_brsh)
L_METHOD(Bit, c_brsh64)
L_METHOD(Bit, c_onBit)
L_METHOD(Bit, c_offBit)
L_METHOD(Bit, c_isBitOn)
LUA_FUNC_END


int Bit::c_bnot(lua_State* L)
{
	int value = (int)lua_tonumber(L, -1);
	lua_pushnumber(L, ~value);
	return 1;
}
int Bit::c_band(lua_State* L)
{
	int rvalue = lua_tointeger(L, -1);
	int lvalue = lua_tointeger(L, -2);
	lua_pushnumber(L, rvalue&lvalue);
	return 1;
}
int Bit::c_band64(lua_State* L)
{
	int64 rvalue = lua_tointeger(L, -1);
	int64 lvalue = lua_tointeger(L, -2);
	lua_pushnumber(L, (LUA_NUMBER)(rvalue&lvalue));
	return 1;
}
int Bit::c_bor(lua_State* L)
{
	int rvalue = lua_tointeger(L, -1);
	int lvalue = lua_tointeger(L, -2);
	lua_pushnumber(L, rvalue|lvalue);
	return 1;
}
int Bit::c_bor64(lua_State* L)
{
	int64 rvalue = lua_tointeger(L, -1);
	int64 lvalue = lua_tointeger(L, -2);
	lua_pushnumber(L, (LUA_NUMBER)(rvalue|lvalue));
	return 1;
}
int Bit::c_bxor(lua_State* L)
{
	int rvalue = lua_tointeger(L, -1);
	int lvalue = lua_tointeger(L, -2);
	lua_pushnumber(L, rvalue^lvalue);
	return 1;
}
int Bit::c_bxor64(lua_State* L)
{
	int64 rvalue = lua_tointeger(L, -1);
	int64 lvalue = lua_tointeger(L, -2);
	lua_pushnumber(L, (LUA_NUMBER)(rvalue^lvalue));
	return 1;
}
int Bit::c_blsh(lua_State* L)
{
	int rvalue = lua_tointeger(L, -1);
	int lvalue = lua_tointeger(L, -2);
	lua_pushnumber(L, rvalue<<lvalue);
	return 1;
}
int Bit::c_blsh64(lua_State* L)
{
	int64 rvalue = lua_tointeger(L, -1);
	int64 lvalue = lua_tointeger(L, -2);
	lua_pushnumber(L, (LUA_NUMBER)(rvalue<<lvalue));
	return 1;
}
int Bit::c_brsh(lua_State* L)
{
	int rvalue = lua_tointeger(L, -1);
	int lvalue = lua_tointeger(L, -2);
	lua_pushnumber(L, rvalue>>lvalue);
	return 1;
}
int Bit::c_brsh64(lua_State* L)
{
	int64 rvalue = lua_tointeger(L, -1);
	int64 lvalue = lua_tointeger(L, -2);
	lua_pushnumber(L, (LUA_NUMBER)(rvalue>>lvalue));
	return 1;
}
int Bit::c_onBit(lua_State* L)
{
	unsigned int num = lua_tointeger(L, 1);
	unsigned int idx = lua_tointeger(L, 2);
	unsigned mask = 1;

	if( idx >= 1 && idx <= 32)
	{
		if (idx > 1)
			mask <<= (idx - 1);
		num |= mask;
		lua_pushinteger(L, num);
		return 1;
	}
	lua_pushinteger(L, num);
	return 1;
}
int Bit::c_offBit(lua_State* L)
{
	unsigned int num = lua_tointeger(L, 1);
	unsigned int idx = lua_tointeger(L, 2);
	unsigned mask = 1;

	if( idx >= 1 && idx <= 32)
	{
		if (idx > 1)
			mask <<= (idx - 1);
		num ^= mask;
		num &= mask;
		lua_pushinteger(L, num);
		return 1;
	}
	lua_pushinteger(L, num);
	return 1;
}
int Bit::c_isBitOn(lua_State* L)
{
	unsigned int num = lua_tointeger(L, 1);
	unsigned int idx = lua_tointeger(L, 2);
	unsigned mask = 1;

	if( idx >= 1 && idx <= 32)
	{
		if (idx > 1)
			mask <<= (idx - 1);
		if (mask & num)
		{
			lua_pushinteger(L, 1);
			return 1;
		}
	}
	lua_pushinteger(L, 0);
	return 1;
}

