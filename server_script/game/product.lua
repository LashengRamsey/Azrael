module("product", package.seeall)

--产品,抽象类
CProduct = class()
function CProduct:__init__(sChineseName, ...)
	self.oFactory = nil
	self.sChineseName = sChineseName --有个中文名,方便调试查找错误
	self.sPriKey = toPriKeyStr(...)
	self.iBirthStamp = 0 --从数据库中加载回来的时间戳
	self.tKeepers = {}
end

function CProduct:addKeeper(oKeeper)
	local oPrx = u.proxy(oKeeper) --避免keeper与product互相循环引用
	table.insertEx(self.tKeepers, oPrx)
end

function CProduct:removeKeeper(oKeeper)
	local oPrx = u.proxy(oKeeper) --避免keeper与product互相循环引用
	table.removeEx(self.tKeepers, oPrx)
end

function CProduct:keeperAmount()
	return table.count(self.tKeepers)
end

function CProduct:this()
	return self
end

function CProduct:getPriKey()--返回主键,类型是tuple
	return self.sPriKey
end

function CProduct:chineseName()
	return self.sChineseName
end

function CProduct:setFactory(oFactory)
	self.oFactory = u.proxy(oFactory)
	return self --可以链式调用
end

function CProduct:getFactory()
	return self.oFactory
end

function CProduct:factoryObj()
    return self.oFactory
end	

function CProduct:birthStamp()
	return self.iBirthStamp
end

function CProduct:setBirthStamp(iBirthStamp)
	self.iBirthStamp = iBirthStamp
end

function CProduct:liveTime()--至今存活时间(进入内存总共多长时间),返回如 94天21时56分5秒 字符串
	return timeU.getTimeStr(timeU.GetSecond()-self.iBirthStamp)
end

function CProduct:_insertToDB(...)
	error('请在子类实现')
end

function CProduct:_deleteFromDB()
	error('请在子类实现')
end

function CProduct:_saveToDB()--执行update语句,bForce为真时不管内存数据有没有发生变化都强行存盘,bForce=False
	error('请在子类实现')
end

function CProduct:_loadFromDB()--执行select语句,bNotExistInsert表示数据库中查不到时是否当场insert
	error('请在子类实现')
end

function CProduct:checkMarkDirty()
	
end