#ifndef TEST_LUA
#define TEST_LUA

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

void init_test();
void test_load_lua();
void test_lua_insert();

#endif // !TEST_LUA
