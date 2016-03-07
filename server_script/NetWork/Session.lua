module("Session", package.seeall)

--一个客户端连接

local gtSessionMap = {}
--local giSessionId = 0

Session = {}
function Session:new(sn)
	local o = {}
	setmetatable(o, self)
	self.__index = self
	self.iSessionId = sn
	self.obj = nil

	return o
end

function Session:SessionId()
	return self.iSessionId
end

function Session:setSessionId(sn)
	self.iSessionId = sn
end

function newSession(sn)
	local sessionObj = Session:new(sn)
	gtSessionMap[sn] = sessionObj
	return sessionObj
end

function delSession(sn)
	if gtSessionMap[sn] then
		gtSessionMap[sn] = nil
	end
end

function delSessionObj(sessionObj)
	if gtSessionMap[sessionObj.iSessionId] then
		gtSessionMap[sessionObj.iSessionId] = nil
	end
end

