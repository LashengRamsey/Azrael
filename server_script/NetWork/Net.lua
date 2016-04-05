module("Net", package.seeall)

C_LuaNetWork = LuaNetwork		--网络
C_SendToServer = C_LuaNetWork.sendToServer
C_SendToNet = C_LuaNetWork.sendToNet
C_SendToDB = C_LuaNetWork.sendToDB
C_SendToGameServer = C_LuaNetWork.sendToGameServer

function doHandlerMsg(src, sn, fid, data, startPos, size)
	print("========doHandlerMsg=============")
	-- print("src = " .. src)
	-- print("sn = " .. sn)
	-- print("fid = " .. fid)
	-- print("startPos = " .. startPos)
	-- print("size = " .. size)
	--print_r("data = " .. data)
	--print(string.len(data))
	G_SetMsgPacket(data, startPos, size)
	
	local protocol = G_UnPacketI(4)
	CLogInfo("LogInfo", "******doHandlerMsg protocol = " .. protocol)
	local func = PacketHandler.getPacketHandler(protocol)
	--print(func)
	if not func then
		CLogInfo("LogInfo", "******doHandlerMsg error not func******* protocol = %d", protocol)
		return
	end
	local sessionObj = Session.getSession(sn)
	local packet = G_UnPacketTable(protocol)

	if sessionObj then
		func(sessionObj, packet)
	else
		func(src, packet)
	end

end


--发送网络包到客户端
function SendPacket(sn)
	C_SendToNet(0, sn, G_NetPacket())
end

--游戏服发到游戏服
function sendToServer(src, sn, protocol, packet)
	G_AddPacket(protocol, packet)
	C_SendToServer(src, 0, sn, G_NetPacket())
end

--游戏服发到DB服
function sendToDB(protocol, packet)
	G_AddPacket(protocol, packet)
	C_SendToDB(0, 20, 0, G_NetPacket())
end

--DB服发到游戏服
function sendToGame(iServerNo, protocol, packet)
	G_AddPacket(protocol, packet)
	C_SendToGameServer(iServerNo, 0, 0, G_NetPacket())
end
