module("Account", package.seeall)


Account = {}
function Account:new(sessionObj, sAccount)
	local o = o or {}
	setmetatable(o, self)
	self.__index = self

	self.SessionObj = sessionObj
	self.sAccount = sAccount
	self.bLoaded = false
	self.bLoading = false
	return o
end

function Account:setSession(sessionObj)
	self.SessionObj = sessionObj
end

function Account:getSession()
	return self.SessionObj
end

function Account:Loaded()
	return self.bLoaded
end

function Account:Loading()
	return self.bLoading 
end

function Account:setLoading(bLoading)
	self.bLoading = bLoading
end

function Account:updateInfo(info)
	print("======updateInfo==========")
	self.bLoaded = true
	self.bLoading = false

	--print_r(info)

	self.iVipExp = info.VipExp
	self.iExp = info.Exp
	self.iGold = info.Gold
	self.iVipLv = info.VipLv
	self.iLv = info.Lv


end

function Account:AccountStr()
	return self.sAccount
end

