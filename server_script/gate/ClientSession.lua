module("ClientSession", package.seeall)

--一个客户端连接
ClientSession = class()
	function ClientSession.__init__(self, sn)
		self.iSessionId = sn
		self.refObj = nil
	end

	function ClientSession.SessionId(self)
		return self.iSessionId
	end

	function ClientSession.setSessionId(self, sn)
		self.iSessionId = sn
	end

	function ClientSession.getRefObj(self)
		return self.refObj
	end

	function ClientSession.setRefObj(self, obj)
		self.refObj = obj
	end


local gtSessionMap = {}
function newSession(sn)
	delSession(sn)
	local sessionObj = ClientSession:new(sn)
	gtSessionMap[sn] = sessionObj
	return sessionObj
end

function delSession(sn)
	local sessionObj = gtSessionMap[sn]
	if sessionObj then
		gtSessionMap[sn] = nil
		if sessionObj.refObj then
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

