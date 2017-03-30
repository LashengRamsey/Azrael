module("Net", package.seeall)
require "SvrType"

C_LuaNetWork = LuaNetwork		--网络
C_SendToServer = C_LuaNetWork.sendToServer
C_SendToNet = C_LuaNetWork.sendToNet
C_SendToDB = C_LuaNetWork.sendToDB
C_SendToGameServer = C_LuaNetWork.sendToGameServer

function gateHandlernetMsg(sn, data, startPos, size)
	--解析协议头，看要向哪个服务发送
	local protocol = G_DataUnPacketI(4, data, startPos, size)
	local svrType = Protocol.whereToSend(protocol)
	local send = {
		sn = sn,
		data = data,
		startPos = startPos,
		size = size,
	}
	Net.sendToServer(1, 0, Protocol.G2S_RevPackage, send)
end

function doHandlerMsg(src, sn, fid, data, startPos, size)
	print("========doHandlerMsg=============")
	G_SetMsgPacket(data, startPos, size)
	local protocol = G_UnPacketI(4)
	CLogInfo("LogInfo", "******doHandlerMsg protocol = " .. protocol)
	local func = PacketHandler.getPacketHandler(protocol)
	if not func then
		CLogInfo("LogInfo", "******doHandlerMsg error not func******* protocol = %d", protocol)
		return
	end
	local packet = G_UnPacketTable(protocol)
	func(src, packet)
end

--客户端网络包
function RecvClientPacket(src, packet)
	local sn = packet.sn or 0
	G_SetMsgPacket(packet.data, packet.startPos, packet.size)
	local protocol = G_UnPacketI(4)
	local func = PacketHandler.getPacketHandler(protocol)
	if not func then
		CLogInfo("LogInfo", "******RecvClientPacket error not func******* protocol = %d", protocol)
		return
	end
	local packet = G_UnPacketTable(protocol)
	local sessionObj = Session.getSession(sn)
	if sessionObj then
		func(sessionObj, packet)
	else
		CLogError("=========RecvClientPacket===not sessionObj=========")
	end
end


--==================================================
--发送网络包到客户端
function SendPacket(sn)
	C_SendToNet(sn, G_NetPacket())
end

--游戏服发到游戏服
function sendToServer(src, sn, protocol, packet)
	G_AddPacket(protocol, packet)
	local t = G_NetPacket()
	print_r(t)
	C_SendToServer(src, sn, t)
end

--游戏服发到DB服
function sendToDB(protocol, packet)
	G_AddPacket(protocol, packet)
	C_SendToDB(0, 20, G_NetPacket())
end

--DB服发到游戏服
function sendToGame(iServerNo, protocol, packet)
	G_AddPacket(protocol, packet)
	C_SendToGameServer(iServerNo, 0, G_NetPacket())
end

--==================================================
