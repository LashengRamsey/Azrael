--module("Packet", package.seeall)

local ARRAY_LEN = 2

--发送网络包
local tNetPacket = {}

--协议头
function G_PacketPrepare(protocol)
	tNetPacket = {}
	table.insert(tNetPacket, {4,protocol})
end


function G_PacketAddI(value, byte)
	value = value or 0
	if not table.containValue({1,2,4,8}, byte) then
		CLogError("ERROR G_PacketAddI byte not in {1,2,4,8}, byte %d", byte)
		return
	end
	table.insert(tNetPacket, {byte, value})
end

function G_PacketAddS(str)
	str = str or ""
	table.insert(tNetPacket, {0, str})
end

function G_NetPacket()
	--G_PrintPacket()
	return tNetPacket
end

function G_PrintPacket()
	print_r(tNetPacket)
end

local STR = "Str"
local ARRAY = "Array"
local gtIntBytes = {
	["Char"] = 1,
	["Short"] = 2,
	["Int"] = 4,
	["Long"] = 8,
}
local ARRAY_BASE = {
	["ArrayChar"] = gtIntBytes.Char,
	["ArrayShort"] = gtIntBytes.Short,
	["ArrayInt"] = gtIntBytes.Int,
	["ArrayLong"] = gtIntBytes.Long,
	["ArrayStr"] = -1,
}



function G_AddPacket(protocol, packet)
	local struct = G_PacketStruct[protocol]
	if not struct then
		CLogError("======G_AddPacket ERROR:not struct protocol=%d", protocol)
		return false
	end
	G_PacketPrepare(protocol)

	local function _addPacket(args, tValue)
		for k, v in pairs(args) do
			local uType = v[1]
			local uValue = tValue[k]
			if uType == STR then
				G_PacketAddS(uValue or "")

			elseif uType == ARRAY then
				local _args = G_PacketStructExe[v[2]]
				uValue = uValue or {}
				local _len = table.count(uValue)
				G_PacketAddI(_len, ARRAY_LEN)
				for _, _v in pairs(uValue) do
					_addPacket(_args, _v)
				end

			elseif table.has_key(ARRAY_BASE, uType) then
				local _bytes = ARRAY_BASE[uType]
				uValue = uValue or {}
				local _len = table.count(uValue)
				G_PacketAddI(_len, ARRAY_LEN)
				if _bytes == -1 then
					for _i = 1, _len do
						G_PacketAddS(uValue[_i])
					end
				else
					for _i = 1, _len do
						G_PacketAddI(uValue[_i], _bytes)
					end
				end

			else
				local byte = gtIntBytes[uType]
				if not byte then
					CLogError("======G_AddPacket ERROR:byte error protocol=%d", protocol)
				end
				uValue = uValue or 0
				G_PacketAddI(uValue, byte)
			end
		end
	end
	_addPacket(struct, packet)
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
		CLogError("ERROR G_UnPacketI byte not in {1,2,4,8}, byte %d", byte)
		return
	end
	
	local temp = string.sub(sMsgPacket, sMsgStarPos+1, sMsgStarPos+byte)
	sMsgStarPos = sMsgStarPos + byte
	local len = string.len(temp)
	local hex = "0x"
	print("len = " .. len)
	print(type(temp))
	for i=len, 1, -1 do
		hex = hex .. string.format("%x", string.byte(temp, i))
		print(string.byte(temp, i))
	end
	-- print(temp)
	print("G_UnPacketI hex = " .. hex)
	local value = C_ToNumber(hex)
	return value or 0
end

function G_UnPacketS()
	local len = G_UnPacketI(2)
	local str = string.sub(sMsgPacket, sMsgStarPos+1, sMsgStarPos+len)
	sMsgStarPos = sMsgStarPos + len
	return str or ""
end

function G_UnPacketTable(protocol)
	--print("========G_UnPacketTable=============")
	local result = {}
	local struct = G_PacketStruct[protocol]
	if not struct then
		CLogError("======G_UnPacketTable ERROR:not struct protocol=%d", protocol)
		return nil
	end

	local function _unPacket(args, _result)
		for k, v in pairs(args) do
			local uType = v[1]
			if uType == STR then
				_result[k] = G_UnPacketS()
			elseif uType == ARRAY then
				local _args = G_PacketStructExe[v[2]]
				_result[k] = {}
				local _len = G_UnPacketI(ARRAY_LEN)
				for _i=1,_len do
					_result[k][_i] = {}
					_unPacket(_args, _result[k][_i])
				end

			elseif table.has_key(ARRAY_BASE, uType) then
				local _bytes = ARRAY_BASE[uType]
				local _len = G_UnPacketI(ARRAY_LEN)
				_result[k] = {}
				if _bytes == -1 then
					for _i = 1, _len do
						table.insert(_result[k], G_UnPacketS())
					end
				else
					for _i = 1, _len do
						table.insert(_result[k], G_UnPacketI(_bytes))
					end
				end

			else
				local byte = gtIntBytes[uType]
				if not byte then
					CLogError("======G_AddPacket ERROR:byte error protocol=%d", protocol)
				end
				_result[k] = G_UnPacketI(byte)
			end
		end
	end
	_unPacket(struct, result)

	--print_r(result)
	return result
end

--==============================================
--网关服用
function G_DataUnPacketI(byte, data, sMsgStarPos, size)
	if byte > size - sMsgStarPos then
		CLogError("ERROR G_UnPacketI byte %d > size %d - sMsgStarPos %d", byte, sMsgStarPos, size)
		return
	end
	if not table.containValue({1,2,4,8}, byte) then
		CLogError("ERROR G_UnPacketI byte not in {1,2,4,8}, byte %d", byte)
		return
	end
	
	local temp = string.sub(data, sMsgStarPos+1, sMsgStarPos+byte)
	--sMsgStarPos = sMsgStarPos + byte
	local len = string.len(temp)
	local hex = "0x"
	for i=len, 1, -1 do
		hex = hex .. string.format("%x", string.byte(temp, i))
	end
	
	local value = C_ToNumber(hex)
	return value or 0
end

--==============================================
--网络包测试
function GetTestSendPacket(sessionObj, packet)
	print("=========GetTestSendPacket=====")
	print_r(packet)
end

function TestSendPacket()
	print("========TestSendPacket=============")
	print("G_ServerNo = " .. G_ServerNo)
	if G_ServerNo == 1 then
		--for i=1,100 do
		local t = 
		{
			-- int11 = -1,--
			int12 = -2,--
			-- int14 = -1,--
			-- int18 = 214748364789,--
			-- str1 = "TestSend Packet",--
			-- int111 = 126,--
			-- int112 = 32766,--
			-- int114 = 2147483646,--
			-- int118 = 214748364786,--
		}

		-- t.array1 = {
		-- 		{
		-- 			int21 = 111,--
		-- 			int22 = 112,--
		-- 			int24 = 114,--
		-- 			int28 = 118,--
		-- 			str2 = "TestSend array 11 Packet",--
		-- 		},
		-- 		{
		-- 			int21 = 121,--
		-- 			int22 = 122,--
		-- 			int24 = 124,--
		-- 			int28 = 128,--
		-- 			str2 = "TestSend array 12 Packet",--
		-- 		},
		-- }

		-- t.array1[1].array2 = {
		-- 			{	int31 = 211,--
		-- 				int32 = 212,--
		-- 				int34 = 214,--
		-- 				int38 = 218,--
		-- 				str3 = "TestSend array 21 Packet",--
		-- 			},
		-- 			{
		-- 				int31 = 221,--
		-- 				int32 = 222,--
		-- 				int34 = 224,--
		-- 				int38 = 228,--
		-- 				str3 = "TestSend array 22 Packet",--
		-- 			}
		-- }
			
		-- t.array1[2].array2 = {
		-- 			{	int31 = 231,--
		-- 				int32 = 232,--
		-- 				int34 = 234,--
		-- 				int38 = 238,--
		-- 				str3 = "TestSend array 23 Packet",--
		-- 			},
		-- 			{
		-- 				int31 = 241,--
		-- 				int32 = 242,--
		-- 				int34 = 244,--
		-- 				int38 = 248,--
		-- 				str3 = "TestSend array 24 Packet",--
		-- 			}
		-- }


		-- t.arrayChar = {1,2,3}
		-- t.arrayShort = {4,5,6}
		-- t.arrayInt = {7,8,9}
		-- t.arrayLong = {10,11,12}
		-- t.arrayStr = {"arrayStr1", "arrayStr2", "arrayStr3"}

		
		Net.sendToServer(1, 0, Protocol.G2G_Test, t)
		--Net.sendToDB(1, 0, 0, 0, Protocol.G2G_Test, t)
		--end
	end
end

function testDb()
	login.login(nil, {AccountStr = "robot1"})
	--dbClient.testQuery()
end
