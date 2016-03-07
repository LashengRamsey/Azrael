--module("Packet", package.seeall)

--发送网络包
local tNetPacket = {}

--协议头
function G_PacketPrepare(protocol)
	tNetPacket = {}
	table.insert(tNetPacket, {4,protocol})
end


function G_PacketAddI(value, byte)
	if not table.containValue({1,2,4,8}, byte) then
		C_Error("ERROR G_PacketAddI byte not in {1,2,4,8}, byte %d", byte)
		return
	end
	table.insert(tNetPacket, {byte, value})
end

function G_PacketAddS(str)
	table.insert(tNetPacket, {0, str})
end

function G_NetPacket()
	return tNetPacket
end

function G_PrintPacket()
	print_r(tNetPacket)
end

--收到的网络包
local sMsgPacket = ""
local sMsgStarPos = 0
local sMsgSize = 0

function G_SetMsgPacket(str, startPos, size)
	sMsgPacket = str
	sMsgStarPos = startPos
	sMsgSize = size
end

function G_UnPacketI(byte)
	if not table.containValue({1,2,4,8}, byte) then
		C_Error("ERROR G_UnPacketI byte not in {1,2,4,8}, byte %d", byte)
		return
	end
	
	local temp = string.sub(sMsgPacket, sMsgStarPos+1, sMsgStarPos+byte)
	sMsgStarPos = sMsgStarPos + byte
	local len = string.len(temp)
	local hex = "0x"
	for i=len, 1, -1 do
		hex = hex .. string.format("%x", string.byte(temp, i))
	end
	
	local value = C_ToNumber(hex)
	return value or 0
end

function G_UnPacketS()
	local len = G_UnPacketI(2)
	local str = string.sub(sMsgPacket, sMsgStarPos+1, sMsgStarPos+len)
	sMsgStarPos = sMsgStarPos + len
	return str
end


--==============================================
--网络包测试
function GetTestSendPacket(sessionObj)
	print("=========start=====")
	--local protocol = G_UnPacketI(4)
	local i1 = G_UnPacketI(1)
	local i2 = G_UnPacketI(2)
	local i4 = G_UnPacketI(4)
	local i8 = G_UnPacketI(8)
	local s = G_UnPacketS()
	--print("protocol = " .. protocol)
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
		G_PacketPrepare(G2CProtocol.G2G_Test)
		G_PacketAddI(127, 1)
		G_PacketAddI(32767, 2)
		G_PacketAddI(2147483647, 4)
		G_PacketAddI(214748364789, 8)
		G_PacketAddS("TestSend Packet")
		G_PacketAddI(126, 1)
		G_PacketAddI(32766, 2)
		G_PacketAddI(2147483646, 4)
		G_PacketAddI(214748364786, 8)
		Net.sendToServer(1, 0, 0, 0)
		--end
	end
end

