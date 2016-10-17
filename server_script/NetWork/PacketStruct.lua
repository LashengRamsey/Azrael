--module("PackMsg", package.seeall)

G_PacketStruct = {}


G_PacketStruct[Protocol.G2S_ClientConn] ={
	sn = {"int64"}
}

G_PacketStruct[Protocol.G2S_ClientDisConn] ={
	sn = {"int64"}
}

G_PacketStruct[Protocol.G2S_RevPackage] ={
	sn = {"int64"},
	data = {"string"},
	startPos = {"int32"},
	size = {"int32"},
}

G_PacketStruct[Protocol.G2G_Test] = 
{
	-- int11 = {"int8"},
	-- int12 = {"int16"},
	-- int14 = {"int32"},
	int18 = {"int64"},
	str1 = {"string"},
	-- int111 = {"int8"},
	-- int112 = {"int16"},
	-- int114 = {"int32"},
	-- int118 = {"int64"},

	-- array1 = {"Array", "Test"},

	-- arrayInt8 = {"ArrayInt8"},
	-- arrayInt16 = {"ArrayInt16"},
	-- arrayInt32t = {"ArrayInt32"},
	-- arrayInt64 = {"ArrayInt64"},
	-- arrayStr = {"ArrayStr"},

}

G_PacketStruct[Protocol.C2G_Login] = 
{
	AccountStr = {"string"}
}

G_PacketStruct[Protocol.G2D_COMMAND] =
{
	iCbId = {"int32"},
	iType = {"int8"},
	db_name = {"string"},
	sValue = {"string"},
}

G_PacketStruct[Protocol.D2G_COMMAND_RESULT] = 
{
	iCbId = {"int32"}, 
	result = {"int8"},
	tResult = {"ArrayStr"}
}





