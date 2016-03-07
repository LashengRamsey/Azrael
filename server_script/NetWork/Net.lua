module("Net", package.seeall)

C_LuaNetWork = LuaNetwork		--网络
C_SendPacket = C_LuaNetWork.sendPacket
C_SendToNet = C_LuaNetWork.sendToNet
C_SendToDB = C_LuaNetWork.sendToDB
C_SendToGameServer = C_LuaNetWork.sendToGameServer

function doHandlerMsg(target, sn, eid, fid, data, startPos, size)
	print("========doHandlerMsg=============")
	print("target = " .. target)
	print("sn = " .. sn)
	print("eid = " .. eid)
	print("fid = " .. fid)
	print("startPos = " .. startPos)
	print("size = " .. size)
	--print_r("data = " .. data)
	--print(string.len(data))
	G_SetMsgPacket(data, startPos, size)
	--GetTestSendPacket()

	local protocol = G_UnPacketI(4)
	CLogInfo("******doHandlerMsg protocol = " .. protocol)

end



function sendToServer(target, fid, sn, uid)
	--print_r(G_NetPacket())
	C_SendToGameServer(target, fid, sn, uid, G_NetPacket())
end

function SendPacket(sn)
	C_SendPacket(-1, 0, sn, 0, G_NetPacket())
end


--==============================================
--网络包测试
function GetTestSendPacket()
	local protocol = G_UnPacketI(4)
	local i1 = G_UnPacketI(1)
	local i2 = G_UnPacketI(2)
	local i4 = G_UnPacketI(4)
	local i8 = G_UnPacketI(8)
	local s = G_UnPacketS()
	print("=========start=====")
	print("protocol = " .. protocol)
	print("i1 = " .. i1)
	print("i2 = " .. i2)
	print("i4 = " .. i4)
	print("i8 = " .. i8)
	print("s = " .. s)
	local i1 = G_UnPacketI(1)
	local i2 = G_UnPacketI(2)
	local i4 = G_UnPacketI(4)
	local i8 = G_UnPacketI(8)
	print("i1 = " .. i1)
	print("i2 = " .. i2)
	print("i4 = " .. i4)
	print("i8 = " .. i8)
	print("=========end=====")
end

function TestSendPacket()
	--print("========TestSendPacket=============")
	print("G_ServerId = " .. G_ServerId)
	if G_ServerId == 1 then
		--for i=1,100 do
		G_PacketPrepare(11111)
		G_PacketAddI(127, 1)
		G_PacketAddI(32767, 2)
		G_PacketAddI(2147483647, 4)
		G_PacketAddI(214748364789, 8)
		G_PacketAddS("TestSend Packet")
		G_PacketAddI(126, 1)
		G_PacketAddI(32766, 2)
		G_PacketAddI(2147483646, 4)
		G_PacketAddI(214748364786, 8)
		sendToServer(1, 0, 0, 0)
		--end
	end
end





