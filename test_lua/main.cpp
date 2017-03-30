#include <stdio.h>
#include "test.h"

int main()
{
	init_test();
	test_load_lua();
	test_lua_insert();
	test_loadConfig();



	return 1;
}