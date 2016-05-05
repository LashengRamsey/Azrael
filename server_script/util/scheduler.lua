
--调度器
--精度很差的,用于定时存盘,对象清出内存等对时间精度要求很低的工作

local CYCLE_TIME=5*60.0 --全部执行一遍所需时间(秒),以此计算出平均每一个执行间隔
--真正的执行间隔受以下两个数修正
local MIN_INTERVAL,MAX_INTERVAL=0.1,5 --最小间隔(秒),最大间隔(秒)

CScheduler = class()
function CScheduler.__init__(self, icycleTime, iMinInterval, iMaxInterval)
	self.icycleTime = icycleTime or CYCLE_TIME
	self.iMinInterval = iMinInterval or MIN_INTERVAL
	self.iMaxInterval = iMaxInterval or MAX_INTERVAL

	self.timerMng = timer.cTimerMng()--用于
	self.deqItems = Struct.deque()--双端队列
	self.dKeyMapFunc = {}--主键映射函数
	self.uTimerId = 0
	self.fNext = self.iMaxInterval --下一次呼叫的间隔
end

function CScheduler.callBackAmount(self)--总共还有多少个callback有待调用
	return table.count(self.dKeyMapFunc)
end

function CScheduler.prependCallLater(self, func, ...)
	self.__addCallLater(False, func, ...)
end

function CScheduler.appendCallLater(self, func, ...)
	self.__addCallLater(True, func, ...)
end

function CScheduler.__addCallLater(self, bAppend, func, ...)
	local sPriKey = toPriKeyStr(...)
	if not sPriKey then
		error('请提供主键.')
		return
	end
	if bAppend then
		--append到尾上就没有必要重复添加,插入到头上就允许重复
		--if tPriKey in self.deqItems--这个可能是遍历,性能稍差
		--	return
		if self.dKeyMapFunc[sPriKey] then--hash性能稳定一点
			return
		end
		self.deqItems:pushBack(sPriKey)
	else--append到前面的插队动作,不检查是否已经在队里,会造成同1个key多次存在队列里
		self.deqItems:pushFront(sPriKey)	
	end		

	self.dKeyMapFunc[sPriKey] = u.makeWeakFunc(func) --存储弱引用
	
	if self.uTimerId == 0 then--定时器尚未起动
		local fDelay = self.__next()
		self.uTimerId = self.timerMng.run(self.__helperFunc,fDelay,timer.NOT_REPEAT,timer.NO_NAME,None,timer.LOWEST)
	end
end

function CScheduler.hasCallLater(self, ...)--是否存在某个回调
	local sPriKey = toPriKeyStr(...)
	if not sPriKey then
		error('请提供主键.')
		return
	end
	return self.dKeyMapFunc[sPriKey] ~= nil
end

function CScheduler.removeCallLater(self, ...)
	local sPriKey = toPriKeyStr(...)
	if not sPriKey then
		error('请提供主键.')
		return
	end
	--if tPriKey in self.deqItems--从双端队列的中间remove元素,性能不知道如何,干脆就不做了
	--	self.deqItems.remove(tPriKey)--时间到了再找一个有效的函数,跳过无效的函数即可
	self.dKeyMapFunc[sPriKey] = nil
end

function CScheduler.__helperFunc(self)
	while self.deqItems do
		sPriKey = self.deqItems:pushFront()--前面弹出来
		func = self.dKeyMapFunc.pop(sPriKey,None)
		if func~=None then--有可能会None,因为会发生多个key存在deqItems里的情况,但dKeyMapFunc只能存一个相同的key
			break
		end
	-- else
	-- 	self.uTimerId = 0 --标识定时器未起动
	-- 	return
	end
	--try--确保不会因为一个调用发生异常,导致执行链中断
		func()
	--except Exception
	--	u.logException()
	--	self.deqItems.append(tPriKey)--出异常了,放回队列尾去,一会再重试
	--	self.dKeyMapFunc[tPriKey]=func --函数也要挂回去

	local fDelay = self.__next()
	if fDelay ~= -1 then--还有元素,继续为下一次执行准备
		if fDelay < self.fNext then--越走越快
			self.fNext = fDelay
		end
		self.uTimerId = self.timerMng.run(self.__helperFunc,self.fNext,timer.NOT_REPEAT,timer.NO_NAME,None,timer.LOWEST)
	else
		self.uTimerId = 0 --标识定时器未起动
		self.fNext = self.iMaxInterval --全部执行完了,恢复初始速度
	end
end
	
function CScheduler.__next(self)
	--iLen=len(self.deqItems)--这个不准确的,以self.dKeyMapFunc为准
	local iLen = len(self.dKeyMapFunc)
	if iLen == 0 then
		return -1 --表示没有元素,不需要定时器了
	end
	local fDelay = self.icycleTime/iLen --平均间隔		
	if fDelay < self.iMinInterval then--不能太密,影响性能
		fDelay = self.iMinInterval 

	elseif fDelay > self.iMaxInterval then--不能太疏,不影响性能的情况下要保证精度
		fDelay = self.iMaxInterval
	end
	return fDelay
end


