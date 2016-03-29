module("dbStruct", package.seeall)


giKeyInt = 1
giKeyStr = 2

gDbTableKey = {
	--表名 = ｛键名，键类型｝
	account = {"Account", giKeyInt},	
	test = 	{"tid", giKeyStr},
}

gDbTableInfo = {}

gDbTableInfo["test"] = 
{
	tid = giKeyInt,
	name = giKeyStr,
}
gDbTableInfo["account"] = 
{
	["Account"] = giKeyStr,
	["Lv"] = giKeyInt,
	["Exp"] = giKeyInt,
	["Gold"] = giKeyInt,
	["VipLv"] = giKeyInt,
	["VipExp"] = giKeyInt,
	["LastLoginTime"] = giKeyInt,
	["CreateTime"] = giKeyInt,
	["DATA"] = giKeyStr,
}


