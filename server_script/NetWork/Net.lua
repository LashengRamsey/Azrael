module("Net", package.seeall)

C_LuaNetWork = LuaNetwork		--网络
C_SendPacket = C_LuaNetWork.sendPacket
C_SendToNet = C_LuaNetWork.sendToNet
C_SendToDB = C_LuaNetWork.sendToDB
C_SendToGameServer = C_LuaNetWork.sendToGameServer

function doHandlerMsg(target, sn, eid, fid, data, startPos, size)
	-- print("========doHandlerMsg=============")
	-- print("target = " .. target)
	-- print("sn = " .. sn)
	-- print("eid = " .. eid)
	-- print("fid = " .. fid)
	-- print("startPos = " .. startPos)
	-- print("size = " .. size)
	--print_r("data = " .. data)
	--print(string.len(data))
	G_SetMsgPacket(data, startPos, size)
	
	local protocol = G_UnPacketI(4)
	CLogInfo("******doHandlerMsg protocol = " .. protocol)
	local func = PacketHandler.getGamePacketHandler(protocol)
	--print(func)
	if not func then
		CLogInfo("******doHandlerMsg error not func*******")
	end
	local sessionObj = Session.getSession(sn)
	--print(session)
	func(sessionObj)
end



function sendToServer(target, fid, sn, uid)
	--print_r(G_NetPacket())
	C_SendToGameServer(target, fid, sn, uid, G_NetPacket())
end

function SendPacket(sn)
	C_SendPacket(-1, 0, sn, 0, G_NetPacket())
end





