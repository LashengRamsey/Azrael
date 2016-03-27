--机器人
module("robot", package.seeall)

C_Connection = Connection 		--连接
local gtConnMap = {}
--print_r(C_Connection)
function newConnection()
	return C_Connection:new()
end

--连接上，底层调用
function onLuaConnect(conn)
	local oConn = gtConnMap[conn]
	if oConn then
		oConn:onConnect()
	end
end

function onLuaClose(conn)
	local oConn = gtConnMap[conn]
	if oConn then
		oConn:onClose()
		gtConnMap[oConn] = nil
	end
end

function onLuaMsg(conn, fid, data, startPos, endPos)
	local oConn = gtConnMap[conn]
	if oConn then
		oConn:onMsg(fid, data, startPos, endPos)
	end
end

function onLuaRawMsg(conn, fid, data, startPos, endPos)
	local oConn = gtConnMap[conn]
	if oConn then
		oConn:onRawMsg(fid, data, startPos, endPos)
	end
end

--机器人对象
robot = {}
local giRobotId = 0
function robot:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	self.SessionObj = nil
	self.uConn = nil


	giRobotId = giRobotId + 1
	self.iRobotId = giRobotId
	return o
end

function robot:setSession(sessionObj)
	self.SessionObj = sessionObj
end

function robot:connect()
	self.uConn = newConnection()
	self.ip = C_GetConfig("GameIp")
	self.port = tonumber(C_GetConfig("GamePort"))
	local notify = 
	{
		onLuaConnect = onLuaConnect,
		onLuaClose = onLuaClose,
		onLuaMsg = onLuaMsg,
		onLuaRawMsg = onLuaRawMsg
	}
	self.uConn:c_Connect(self.ip, self.port, notify, 0, true)
	gtConnMap[self.uConn] = self
	self.bIsConn = true
end

function robot:onConnect()
	CLogInfo("=======robot:onConnect=========")
	self:sendLogin()
end

function robot:Write()

end

function robot:Close()
	if self.uConn then
		self.uConn:c_Close()
	end
end

function robot:onClose()
	CLogInfo("=======robot:onClose=========")
	self.uConn = nil
	self.bIsConn = false
end

function robot:IsConnect()
	if not self.uConn then
		return
	end
	return self.uConn:c_IsConnect()
end

function robot:RawWrirte(str)
	if not self.uConn then
		return false
	end

	self.uConn:c_RawWrirte(str)
end

function robot:onMsg(fid, data, startPos, endPos)
	CLogInfo("=======robot:onMsg=========")
end

function robot:onRawMsg(fid, data, startPos, endPos)
	CLogInfo("=======robot:onRawMsg=========")
end


function robot:update()
	if not self.bIsConn then
		self:connect()
	end
end

function robot:sendLogin()
	CLogInfo("=======robot:sendLogin=========")
	local packet = {}
	packet.AccountStr = "robot" .. self.iRobotId

	G_AddPacket(Protocol.C2G_Login, packet)
	self.uConn:c_Write(0, G_NetPacket())
end

