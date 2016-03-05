--module("Connect", package.seeall)

C_Connection = Connection 		--连接

local giConnFd = nil
local guConn = nil

Connect = {}
function Connect:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self

	self.iConnFd = 0
	self.uConn = nil
	return o
end

function Connect:newConn()
	return C_Connection:new(self)
end

function Connect:Connect(ip, port, notify, timeout, raw)
	self.uConn = self:newConn()
	self.iConnFd = self.uConn:c_Connect(ip, port, notify, timeout, raw)
	print(" Connect:Connect self.iConnFd = " .. self.iConnFd)
	CLogInfo("Connect:Connect success self.iConnFd = %d", self.iConnFd)

end

--连接上，底层调用
function Connect:onLuaConnect()
	-- for i = 1, select('#', ...) do
	-- 	print("==Connect====onConnect============")
 --        local arg = select(i, ...)
 --       	print("arg", arg)
 --    end  
	 CLogInfo("===Connect:onConnect=====")
	-- print(self.uConn)
	--print(" Connect:onLuaConnect self.iConnFd = " .. self.iConnFd)
end

function Connect:Write()

end

function Connect:Close()
	if self.uConn then
		self.uConn:c_Close()
	end
end

function Connect:onLuaClose(conn)
	print(conn)
	--self.iConnFd = 0
	--self.uConn = nil
	--print(" Connect:onLuaClose self.iConnFd = " .. self.iConnFd)
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


function Connect:onLuaMsg(fid, data, startPos, endPos)
	print("=========Connect:onLuaMsg=========")
end

function Connect:onLuaRawMsg(fid, data, startPos, endPos)
	print("=======onnect:onLuaRawMsg======")
end

local testConn = nil
function Connect_test()
	print_r(C_Connection)
	testConn = Connect:new()
	if G_ServerId == 1 then
		local notify = 
		{
			onLuaConnect = Connect.onLuaConnect,
			onLuaClose = Connect.onLuaClose,
			onLuaMsg = Connect.onLuaMsg,
			onLuaRawMsg = Connect.onLuaRawMsg
		}
		testConn:Connect("127.0.0.1", 7801, notify, 100, false)
	end
end



