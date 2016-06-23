module("PacketHandler", package.seeall)

require "gateService"


gtPacketHandler = {}

function getPacketHandler(protocol)
	return gtPacketHandler[protocol]
end

function initComPacketHandler()
	gtPacketHandler[Protocol.G2G_Test] = GetTestSendPacket
end

--===============================================
--gate
function getGatePacketHandler( protocol )
	return gtPacketHandler[protocol]
end

function initGatePacketHandler()
	--gtPacketHandler[Protocol.S2G_SendPackage] = login.login
end

--===============================================
--===============================================
--game
function getGamePacketHandler( protocol )
	return gtPacketHandler[protocol]
end

function initGamePacketHandler()
	initComPacketHandler()
	gtPacketHandler[Protocol.G2S_ClientConn] = gateService.GameClientConnection
	gtPacketHandler[Protocol.G2S_ClientDisConn] = gateService.GameClientDisConn
	gtPacketHandler[Protocol.G2S_RevPackage] = Net.RecvClientPacket

	gtPacketHandler[Protocol.C2G_Login] = login.login
	gtPacketHandler[Protocol.D2G_COMMAND_RESULT] = dbClient.CommandCallBack
	
end
--===============================================
--===============================================
--db
function getDbPacketHandler(protocol)
	return gtPacketHandler[protocol]
end

function initDbPacketHandler()
	initComPacketHandler()
	gtPacketHandler[Protocol.G2D_COMMAND] = dbServer.G2DExeCommand

end

--===============================================
--gate


--===============================================
