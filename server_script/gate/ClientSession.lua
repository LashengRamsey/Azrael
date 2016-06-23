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
	closeSession(sn)
	local sessionObj = ClientSession(sn)
	gtSessionMap[sn] = sessionObj

	--通知各个服务器有新建立了一个连接
	for _,src in pairs({1}) do 	--todo  写死了服务ID
		Net.sendToServer(src, 0, Protocol.G2S_ClientConn, {sn = sn})
	end

	return sessionObj
end

function closeSession(sn)
	local sessionObj = gtSessionMap[sn]
	if sessionObj then
		gtSessionMap[sn] = nil
		for _,src in pairs({1}) do 	--todo  写死了服务ID
			Net.sendToServer(src, 0, Protocol.G2S_ClientDisConn, {sn = sn})
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

