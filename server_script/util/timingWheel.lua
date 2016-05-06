module("timingWheel", package.seeall)

--时间轮盘
--每n分钟给角色回复一定体力
--角色断线n分钟后踢出内存
--对时间要求不是非常精准的

class = CTimingWheel()
function CTimingWheel.__init__(self, iScaleAmount, iInterval)
	if iScaleAmount <= 0 then
		err('iScaleAmount至少是1.')
	end
	self.iScaleAmount = iScaleAmount or 8 --刻度数量(bucket数量,桶数量)
	self.iInterval = iInterval or 3 --每刻度停留秒数(可以理解成是定时器的误差值.)
	self.dBucket = {}
	self.iScale = 0
	
	--self.timerMng = timer.cTimerMng()--
	self.uTimerId = 0
	self.dKeyMapScale = {}

	--self.oLock = gevent.lock.RLock()
end

function CTimingWheel.callbackAmount(self)
	return table.count(self.dKeyMapScale)
end

function CTimingWheel.addCallback(self, cCallback, ...)--增加一个回调函数(重复添加会覆盖)
	local sPriKey = toPriKeyStr(...)
	if not sPriKey then
		err("请提供主键")
	end
	if self.uTimerId == 0 then--尚未启动定时器
		self.uTimerId = timer.CallLater(self.__helperFunc, self.iInterval, None, timer.NOT_REPEAT, timer.NO_NAME)
	end
	
	self.removeCallback(sPriKey)
	cCallback = u.weakRef(cCallback)--建立弱引用
	self.dKeyMapScale[sPriKey] = self.iScale
	if not self.dBucket[self.iScale] then
		self.dBucket[self.iScale] = {}
	end
	self.dBucket[self.iScale][sPriKey] = cCallback	
end
	
function CTimingWheel.hasCallback(self, ...)--是否有一个回调函数	
	local sPriKey = toPriKeyStr(...)
	if not sPriKey then
		err("请提供主键")
	end
	return self.dKeyMapScale[sPriKey] ~= nil
end

function CTimingWheel.removeCallback(self, ...)--移除一个回调函数		
	local sPriKey = toPriKeyStr(...)
	if not sPriKey then
		err("请提供主键")
	end
	iScale = self.dKeyMapScale[sPriKey]
	if self.dBucket[iScale] then
		self.dBucket[iScale][sPriKey] = nil
	end
end

function CTimingWheel.__helperFunc(self)
	--with self.oLock
	self.uTimerId = 0 --标志尚未启动定时器
	self.iScale = self.iScale + 1
	if self.iScale >= self.iScaleAmount then
		self.iScale = 0
	end

	if self.dBucket[self.iScale] then
		for sPriKey, func in pairs(self.dBucket[self.iScale]) do
			self.dKeyMapScale[sPriKey] = nil
			func()
		end
	end

	if table.count(self.dBucket) > 0 and self.uTimerId==0 then--启动下一次定时器
		--self.uTimerId=self.timerMng.run(self.__helperFunc,self.iInterval,timer.NOT_REPEAT,timer.NO_NAME,None,timer.LOWEST)
		self.uTimerId = timer.CallLater(self.__helperFunc, self.iInterval, None, timer.NOT_REPEAT, timer.NO_NAME)
	end
end
