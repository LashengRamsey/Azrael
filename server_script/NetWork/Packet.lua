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
	--local temp = string.reverse(string.sub(sMsgPacket, sMsgStarPos+1, sMsgStarPos+byte))
	--print("sMsgStarPos len = " .. sMsgStarPos)
	local temp = string.sub(sMsgPacket, sMsgStarPos+1, sMsgStarPos+byte)
	sMsgStarPos = sMsgStarPos + byte
	local len = string.len(temp)
	local hex = "0x"
	for i=len, 1, -1 do
	--for i=1, len do
		--print(string.byte(temp, i))
		--print(string.format("%x", string.byte(temp, i)))
		hex = hex .. string.format("%x", string.byte(temp, i))
	end
	--print(hex)
	local value = tonumber(hex)
	print("value = " .. value)
	return value or 0
end

function G_UnPacketS()
	local len = G_UnPacketI(2)
	local str = string.sub(sMsgPacket, sMsgStarPos+1, sMsgStarPos+len)
	sMsgStarPos = sMsgStarPos + len
	return str
end
