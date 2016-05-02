module("BlockBase", package.seeall)


--数据块,抽象类
CBlock = class(product.CProduct)
function CBlock:__init__(sChineseName, ...)
	product.cProduct:__init__(sChineseName, ...)
	self.sSlStm = self.sUdStm = self.sIsStm = self.sDlStm = '' --增删改查的SQL语句
	self.sBackup = nil --检查程序员是否漏打脏标志,内服才启用,外服不能用,因为性能差
	self.bInitialized = false --是否初始化完成
end

function CBlock:isInitialized()--是否初始化完成
	return self.bInitialized
end

function CBlock:setIsStm(sIsStm)--insert statement
	self.sIsStm = sIsStm
	return self --可以链式调用
end

function CBlock:getIsStm()
	return self.sIsStm
end

function CBlock:setDlStm(sDlStm)--delete statement
	self.sDlStm = sDlStm
	return self --可以链式调用
end

function CBlock:getDlStm()
	return self.sDlStm
end

function CBlock:setUdStm(sUdStm)--update statement
	self.sUdStm = sUdStm
	return self --可以链式调用
end

function CBlock:getUdStm()
	return self.sUdStm
end

function CBlock:setSlStm(sSlStm)--select statement
	self.sSlStm = sSlStm
	return self --可以链式调用
end

function CBlock:getSlStm()
	return self.sSlStm
end

function CBlock:getPstObj()
	return self
end

function CBlock:_insertToDB(*itNoRowInsertValues)--override
	local oPst = self:getPstObj()
	oPst:onBorn()	--初始化新生数据
	local dData=oPst.save()
	self.sBackup=ujson.dumps(dData)
	--oPst.markDirty()--(这里不打脏标记,由onBorn里面来决定是否打脏标记)
	local values=[]
	values.extend(self.tPriKey)
	values.append(self.sBackup)
	db4mainService.gConnectionPool.query(self.sIsStm,*values)
	
	self.bInitialized=True
	self._onInitialized()
end

function CBlock:_deleteFromDB()--override
	db4mainService.gConnectionPool.query(self.getDlStm(),*self.tPriKey)
end

function CBlock:checkMarkDirty()--override 本次存盘数据与上一次的存盘数据对比(只在内服检查程序员的错误)
	if not config.IS_INNER_SERVER
		return
	local dData=self.getPstObj().save()
	local sData=ujson.dumps(dData)
	if self.sBackup is not None and not self._equal(self.sBackup,sData)
		try
			raise Exception,'\'{}\'数据发生了变化,但是漏打脏标志\nsBackup={},sData={}'.format(self.sChineseName,self.sBackup,sData)
		except Exception
			misc.logException()
end

function CBlock:_equal(sBackup,sData)--判断sBackup和sData是否相同
	return sBackup==sData --or ujson.loads(sBackup)==ujson.loads(sData)
end

function CBlock:_saveToDB()--override 执行update语句
	local oPst=self.getPstObj()
	local dData=oPst.save()
	local sData=ujson.dumps(dData)
	db4mainService.gConnectionPool.query(self.getUdStm(),sData,*self.tPriKey)		
	if config.IS_INNER_SERVER
		self.sBackup=sData
	return True
end

function CBlock:_loadFromDB()--override 执行select语句
	rs=db4mainService.gConnectionPool.query(self.getSlStm(),*self.tPriKey)
	--print 'rs.rows==',rs.rows
	if len(rs.rows)>1
		raise Exception,'行数过多,返回结果集应该只有1行'
	elseif len(rs.rows)<1--数据库中没有此行			
		return False

	if len(rs.rows[0])!=1
		raise Exception,'列数只能是1列'
	sData=rs.rows[0][0]
	if sData
		try
			dData=ujson.loads(sData)--反序列化
		except Exception
			u.reRaise('反序列化\'{}\'数据块时出错,主键为{}'.format(self.sChineseName,self.getPriKey()))
	else
		dData={}

	oPst=self.getPstObj()
	oPst.load(dData)
	
	if config.IS_INNER_SERVER
		self.sBackup=ujson.dumps(oPst.save())  -- save可能新加了一些标记
	
	self.bInitialized=True
	self._onInitialized()
	return True
end

function CBlock:_onInitialized()
	pass
end

--cCtnBlock,各个容器的基类,因为容器对象sava时由于字典访问的无序性,所以可能造成dBackup,dData里面的数据项一样,
--但数据项排列不一样,是的sBackup和sData不同
--此时若sBackup,sData里的数据项相同既可认为它们相等
CCtnBlock = class(cBlock)
function CCtnBlock:__in99999it__(sChineseName,*tPriKey)
	cBlock.__in99999it__( sChineseName, *tPriKey)
end
	
function CCtnBlock:_equal999999(sBackup,sData)
	if sBackup==sData
		return True
	print 'sChineseName=',u.trans(self.sChineseName)
	print 'sBackup=',sBackup
	print 'sData=',sData
	dBackup,dData=ujson.loads(sBackup),ujson.loads(sData)
	lBackItem,lCurrItem=dBackup.get('item', []), dData.get('item', [])
	if len(lBackItem)!=len(lCurrItem)
		return False
	--判断lBackItem,lCurrItem里面的数据是否相同,此处性能较差
	--只有在内网角色下线时 或者从keeper移除该ctn时,才会进入此函数
	sPos=set()
	for uData in lBackItem	
		if uData not in lCurrItem
			return False
		sPos.add(lBackItem.index(uData))
	return len(sPos)==len(lCurrItem)
end




