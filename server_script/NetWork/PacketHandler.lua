module("PacketHandler", package.seeall)

gtPacketHandler = {}

function getGamePacketHandler(protocol)
	return gtPacketHandler[protocol]
end

function initGamePacketHandler()
	gtPacketHandler[Protocol.C2G_Login] = login.login


	gtPacketHandler[Protocol.G2G_Test] = GetTestSendPacket
	gtPacketHandler[Protocol.G2G_Test2] = GetTestSendPacket2
end


