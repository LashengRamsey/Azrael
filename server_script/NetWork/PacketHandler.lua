module("PacketHandler", package.seeall)

gtPacketHandler = {}

function getGamePacketHandler(protocol)
	return gtPacketHandler[protocol]
end

function initGamePacketHandler()
	gtPacketHandler[C2GProtocol.C2G_Login] = login.login


	gtPacketHandler[G2CProtocol.G2G_Test] = GetTestSendPacket
end


