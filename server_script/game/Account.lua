module("Account", package.seeall)

Account = {}
function Account:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self

	self.SessionObj = nil
	
	return o
end





