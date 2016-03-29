module("PacketHandler", package.seeall)

gtPacketHandler = {}

function getPacketHandler(protocol)
	return gtPacketHandler[protocol]
end

function initComPacketHandler()
	gtPacketHandler[Protocol.G2G_Test] = GetTestSendPacket
end

function initGamePacketHandler()
	initComPacketHandler()

	gtPacketHandler[Protocol.C2G_Login] = login.login
	gtPacketHandler[Protocol.D2G_COMMAND_RESULT] = dbClient.CommandCallBack
	
end

function getDbPacketHandler(protocol)
	return gtPacketHandler[protocol]
end

function initDbPacketHandler()
	initComPacketHandler()
	gtPacketHandler[Protocol.G2D_COMMAND] = dbServer.G2DExeCommand

end


