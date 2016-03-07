module("PacketHandler", package.seeall)

local gtPacketHandler = {}

function initPacketHandler()
	gtPacketHandler[C2GProtocol.C2G_Login] = login.login
end


