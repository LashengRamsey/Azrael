module("serverToGate", package.seeall)


local gtConnMap = {}
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

--=============================================================
--跟网关的连接对象
local CGateSession = class()
	function CGateSession.__init__(self, sIp, iPoint)
		self.c_Conn = nil
	end

	function CGateSession.connect(self)
		self.c_Conn = newConnection()
		self.ip = C_GetConfig("GameIp")
		self.port = tonumber(C_GetConfig("GamePort"))
		local notify = 
		{
			onLuaConnect = onLuaConnect,
			onLuaClose = onLuaClose,
			onLuaMsg = onLuaMsg,
			onLuaRawMsg = onLuaRawMsg
		}
		self.c_Conn:c_Connect(self.ip, self.port, notify, 0, true)
		gtConnMap[self.c_Conn] = self
		self.bIsConn = true
	end

	function CGateSession.onConnect(self)
		CLogInfo("LogInfo", "=======CGateSession:onConnect=========")
		self:sendLogin()
	end

	function CGateSession.Close(self)
		if self.c_Conn then
			self.c_Conn:c_Close()
		end
	end

	function CGateSession.onClose(self)
		CLogInfo("LogInfo", "=======CGateSession.onClose=========")
		self.c_Conn = nil
		self.bIsConn = false
	end

	function CGateSession.IsConnect(self)
		if not self.c_Conn then
			return
		end
		return self.c_Conn:c_IsConnect()
	end

	function CGateSession.RawWrirte(self, str)
		if not self.c_Conn then
			return false
		end

		self.c_Conn:c_RawWrirte(str)
	end

	function CGateSession.onMsg(self, fid, data, startPos, endPos)
		CLogInfo("LogInfo", "=======robot.onMsg=========")
	end

	function CGateSession.onRawMsg(self, fid, data, startPos, endPos)
		CLogInfo("LogInfo", "=======robot.onRawMsg=========")
	end


	function CGateSession.update(self)
		if not self.bIsConn then
			self:connect()
		end
	end

	function CGateSession.sendLogin(self)
		CLogInfo("LogInfo", "=======robot.sendLogin=========")
		local packet = {}
		packet.AccountStr = "robot" .. self.iRobotId

		G_AddPacket(Protocol.C2G_Login, packet)
		self.uConn:c_Write(0, G_NetPacket())
	end

--=============================================================
local gtGatePort = {}
function initGatePort()
	gtGatePort = {}
	local sGatePort = C_GetConfig("GatePort")
	local tPort = string.split(sGatePort, ";")
	for _,v in pairs(tPort) do
		local _t = string.split(v, ":")
		table.insert(gtGatePort, {_t[1], tonumber(_t[2])})
	end
end

--服务启动后，各服连接到网关
function connectToGate(iServerType)
	initGatePort()
	
	for ip,port in pairs(gtGatePort) do

	end
end

