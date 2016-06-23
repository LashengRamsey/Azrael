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
	self.refObj = nil

	return o
end

function Session:SessionId()
	return self.iSessionId
end

function Session:setSessionId(sn)
	self.iSessionId = sn
end

function Session:getRefObj()
	return self.refObj
end

function Session:setRefObj(obj)
	self.refObj = obj
end

function newSession(sn)
	local sessionObj = Session:new(sn)
	gtSessionMap[sn] = sessionObj
	return sessionObj
end

function delSession(sn)
	local sessionObj = gtSessionMap[sn]
	if sessionObj then
		gtSessionMap[sn] = nil
		if sessionObj.refObj and sessionObj.refObj.onDisconnect then
			sessionObj.refObj:onDisconnect()
		end
	end
end

function getSession(sn)
	return gtSessionMap[sn]
end

function delSessionObj(sessionObj)
	if gtSessionMap[sessionObj.iSessionId] then
		gtSessionMap[sessionObj.iSessionId] = nil
	end
end

