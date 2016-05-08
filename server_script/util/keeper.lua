module('keeper', package.seeall)
--对象管理器

--key映射obj
--作用是当调用getObj拿到是proxy
--确保obj的ownership是属于这个keeper,不会发生ownership转移或被共享
CKeeper = class()
function CKeeper:__init__()
	self.dProxy = {} --实例的代理
	self.dObjs = {} --实例的强引用
end

function CKeeper:getObj(...)--返回proxy
	local sPriKey = toPriKeyStr(...)
	if not sPriKey then
		error('请提供主键.')
		return
	end
	
	return self.dProxy[sPriKey]
end

function CKeeper:addObj(obj, ...)
	local sPriKey = toPriKeyStr(...)
	if not sPriKey then
		error('请提供主键.')
		return
	end
	if self.dProxy[sPriKey] then
		return
	end
	--raise Exception,'{}为key的对象已在管理器中了{}.'.format(tPriKey,obj)
	self.dProxy[sPriKey] = r.proxy(obj)
	self.dObjs[sPriKey] = obj
end

function CKeeper:removeObj(...)
	local sPriKey = toPriKeyStr(...)
	if not sPriKey then
		error('请提供主键.')
		return
	end
	--下面两句有顺序的,先从proxy弹出,避免从dObjs弹出进引起的析构函数访问dProxy里面的元素,
	--引起ReferenceError weakly-referenced object no longer exists			
	self.dProxy[sPriKey] = nil
	self.dObjs[sPriKey] = nil
end

function CKeeper:removeAllObj()
	self.dProxy = {}
	self.dObjs = {}
end
			
function CKeeper:amount()
	return table.count(self.dObjs)
end

function CKeeper:getItems()
	return self.dProxy.items()
end

function CKeeper:getKeys()
	return table.keys(self.dProxy)
end

function CKeeper:getValues()
	return table.values(self.dProxy)
end


