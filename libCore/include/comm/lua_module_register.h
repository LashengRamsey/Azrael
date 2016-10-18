#ifndef _LUA_MODULE_REGISTER_H__
#define _LUA_MODULE_REGISTER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"

LUALIB_API int luaopen_struct(lua_State *L);
int luaopen_pb(lua_State *L);

#ifdef __cplusplus
}
#endif

#endif  // _LUA_MODULE_REGISTER_H__

