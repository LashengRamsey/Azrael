--module("PackMsg", package.seeall)

G_PacketStruct = {}

G_PacketStruct[Protocol.G2G_Test] = 
{
	int11 = {"Char"},
	int12 = {"Short"},
	int14 = {"Int"},
	int18 = {"Long"},
	str1 = {"Str"},
	int111 = {"Char"},
	int112 = {"Short"},
	int114 = {"Int"},
	int118 = {"Long"},

	array1 = {"Array", "Test"},

	arrayChar = {"ArrayChar"},
	arrayShort = {"ArrayShort"},
	arrayInt = {"ArrayInt"},
	arrayLong = {"ArrayLong"},
	arrayStr = {"ArrayStr"},

}

G_PacketStruct[Protocol.C2G_Login] = 
{
	AccountStr = {"Str"}
}

G_PacketStruct[Protocol.G2D_COMMAND] =
{
	iCbId = {"Int"},
	db_name = {"Str"},
	id = {"Str"},
}

G_PacketStruct[Protocol.D2G_COMMAND_RESULT] = 
{
	iCbId = {"Int"}, 
	result = {"Char"},
	sResult = {"Str"}
}








