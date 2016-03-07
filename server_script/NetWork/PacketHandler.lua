module("PacketHandler", package.seeall)

local gtPacketHandler = {}

function initGamePacketHandler()
	gtPacketHandler[C2GProtocol.C2G_Login] = login.login
end


