#ifndef LUABIT_H
#define LUABIT_H

#include "lunar.h"

class Bit
{
public:
	LUA_EXPORT(Bit)

	//export function
	static int c_bnot(lua_State* L);
	static int c_band(lua_State* L);
	static int c_band64(lua_State* L);
	static int c_bor(lua_State* L);
	static int c_bor64(lua_State* L);
	static int c_bxor(lua_State* L);
	static int c_bxor64(lua_State* L);
	static int c_blsh(lua_State* L);
	static int c_blsh64(lua_State* L);
	static int c_brsh(lua_State* L);
	static int c_brsh64(lua_State* L);
	static int c_onBit(lua_State* L);
	static int c_offBit(lua_State* L);
	static int c_isBitOn(lua_State* L);
};


#endif //!LUABIT_H

