module("dbStruct", package.seeall)


giKeyInt = 1
giKeyChar = 2

gDbTableKey = {
	--表名 = ｛键名，键类型｝
	account = {"Account", giKeyInt},	
	test = 	{"tid", giKeyChar},
}

gDbTableInfo = {}

gDbTableInfo["test"] = 
{
	tid = {"Int"},
	name = {"Str"}
}
gDbTableInfo["account"] = 
{

}


