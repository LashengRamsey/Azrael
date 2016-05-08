module("product", package.seeall)

--系统中全部的Product keeper实例
local gtProductKeeper = gtProductKeeper or {}

--product对象keeper.肚子里装的是product对象
CProductkeeper = class(keeper.CKeeper)
function CProductkeeper.__init__(self, oFactory)
	keeper.cKeeper.__init__(self)		
	self.oFactory = oFactory
	self.tLock={}	--key映射锁,用于互斥访问(相同key的才互斥,不同key可以并发)
	table.insert(gtProductKeeper, self)
end

function CProductkeeper.getObjWithLock(self, ...)
	return keeper.cKeeper.getObj(self, ...)
end

function CProductkeeper.getObjFromDB(self, itNoRowInsertValues, ...)
	--从数据库加载对象,并且交给这个keeper进行管理
	local obj = self.getObj(...)
	if obj then--已经在内存中了.
		return obj
	end
	obj=self.oFactory.getProductFromDB(itNoRowInsertValues, ...)--从工厂生产obj
	if not obj then
		return None
	end
	self.addObj(obj, ...)	
	return self.getObj(...)--返回一个代理出去.
end

function CProductkeeper.addObj(self, obj, ...)--override
	if not isInstance(obj, product.CProduct) then
		err('必须是product的实例才能加入到keeper里.')
	end
	--if isInstance(obj, weakref.ProxyType) then
	--	obj = obj.this()--从proxy取出直实的实例
	--end
	keeper.cKeeper.addObj(self,obj, ...)
	obj.addKeeper(self)		
end
	
function CProductkeeper.removeObj(self, ...)--override
	local obj = self.getObj(...)
	if obj then
		obj.removeKeeper(self)
		if obj.keeperAmount() <= 0
			--没有任何keeper持有这个实例了.(事实上可能有其他地方还有强引用这个product对象,不过不是keeper.比如friend里面强引用了resume)
			--if config.IS_INNER_SERVER
			--	print u.trans2gbk('在keeper中呆了{},即将踢出keeper,最后一次存盘.主键是{}.obj={}'.format(obj.liveTime(),tPriKey,obj))
			if self.oFactory.isWait2schedule(...) then
				--是否正在等待存盘调度
				self.oFactory.saveProduct2db(...)
				--最后一次存盘了.(里面会从存盘队列里移除自己)
			else
				obj.checkMarkDirty()
			end
		end
	end
	keeper.cKeeper.removeObj(self, ...)
end

function CProductkeeper.removeAllObj(self)--override,基类仅仅是置空,要改为逐个移除
	for tPriKey in self.dObjs.keys()
		self.removeObj(*tPriKey)
	end
end
