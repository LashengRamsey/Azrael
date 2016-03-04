--module("Connect", package.seeall)

C_Connection = Connection 		--连接

local giConnFd = nil
local guConn = nil

Connect = {}
function Connect:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end

function Connect:newConn()
	return C_Connection:new(self)
end

function Connect:Connect(ip, port, notify, timeout, raw)
	self.guConn = self:newConn()
	giConnFd = self.guConn:c_connect(ip, port, notify, timeout, raw)
	CLogInfo("Connect:Connect success giConnFd = %d", giConnFd)

end

--底层调用
function Connect:onConnect(Conn)
	CLogInfo("===Connect:onConnect=====")
	guConn = Conn
	print(guConn)
end

function onConnect(Conn)
	CLogInfo("===onConnect=====")
	guConn = Conn
	print(guConn)
end

function Write()

end

function Close()

end

function IsConnect()

end

function RawWrirte()

end


local testConn = nil
function Connect_test()
	print_r(C_Connection)
	--print(C_Connection.c_connect)
	testConn = Connect:new()
	if G_ServerId == 1 then
		testConn:Connect("127.0.0.1", 7801, {Connect}, 0, false)
	end
end



