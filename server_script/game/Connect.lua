module("Connect", package.seeall)

C_Connection = Connection 		--连接


local gtConnMap = {}
local giConnId = 0

--连接上，底层调用
function onLuaConnect(conn)
	local oConn = gtConnMap[conn]
	oConn:onConnect()
end

function onLuaClose(conn)
	local oConn = gtConnMap[conn]
	oConn:onClose()
end

function onLuaMsg(conn, fid, data, startPos, endPos)
	local oConn = gtConnMap[conn]
	oConn:onMsg(fid, data, startPos, endPos)
end

function onLuaRawMsg(conn, fid, data, startPos, endPos)
	local oConn = gtConnMap[conn]
	oConn:onRawMsg(fid, data, startPos, endPos)
end



Connect = {}
function Connect:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self

	self.iConnFd = 0
	self.uConn = nil
	return o
end

function newConnection()
	return C_Connection:new()
end

function Connect:Connect(ip, port, notify, timeout, raw)
	self.uConn = newConnection()
	self.iConnFd = self.uConn:c_Connect(ip, port, notify, timeout, raw)
	print(self.uConn)
	print(self)
	gtConnMap[self.uConn] = self

	CLogInfo("Connect:Connect success self.iConnFd = %d", self.iConnFd)
end

function Connect:onConnect()
	CLogInfo("=======Connect:onConnect=========")
end

function Connect:Write()

end

function Connect:Close()
	if self.uConn then
		self.uConn:c_Close()
	end
end

function Connect:onClose()
	CLogInfo("=======Connect:onClose=========")
	gtConnMap[self.uConn] = nil
end

function Connect:IsConnect()
	if not self.uConn then
		return
	end
	return self.uConn:c_IsConnect()
end

function Connect:RawWrirte(str)
	if not self.uConn then
		return false
	end

	self.uConn:c_RawWrirte(str)
end

function Connect:onMsg(fid, data, startPos, endPos)
	CLogInfo("=======Connect:onMsg=========")
end

function Connect:onRawMsg(fid, data, startPos, endPos)
	CLogInfo("=======Connect:onRawMsg=========")
end


local testConn = nil
function Connect_test()
	print_r(C_Connection)
	testConn = Connect:new()
	if G_ServerId == 1 then
		local notify = 
		{
			onLuaConnect = onLuaConnect,
			onLuaClose = onLuaClose,
			onLuaMsg = onLuaMsg,
			onLuaRawMsg = onLuaRawMsg
		}
		testConn:Connect("127.0.0.1", 7801, notify, 10, false)
	end
end



