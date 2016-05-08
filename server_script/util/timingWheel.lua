module("timingWheel", package.seeall)

--时间轮盘
--每n分钟给角色回复一定体力
--角色断线n分钟后踢出内存
--对时间要求不是非常精准的

CTimingWheel = class()
function CTimingWheel.__init__(self, iScaleAmount, iInterval)
	if iScaleAmount <= 0 then
		err('iScaleAmount至少是1.')
	end
	self.iScaleAmount = iScaleAmount or 8*1000 --刻度数量(bucket数量,桶数量)
	self.iInterval = iInterval or 3*1000 --每刻度停留秒数(可以理解成是定时器的误差值.)
	self.tBucket = {}
	self.iScale = 1
	
	self.uTimerId = 0
	self.dKeyMapScale = {}
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
		self.uTimerId = timer.CallLater(handler(self, self.__helperFunc), self.iInterval, None, timer.NOT_REPEAT, timer.NO_NAME)
		--print("CTimingWheel.addCallback self.uTimerId = " .. self.uTimerId)
	end
	
	self:removeCallback(...)
	--cCallback = u.weakRef(cCallback)--建立弱引用
	self.dKeyMapScale[sPriKey] = self.iScale
	if not self.tBucket[self.iScale] then
		self.tBucket[self.iScale] = {}
	end
	self.tBucket[self.iScale][sPriKey] = cCallback	
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
	local iScale = self.dKeyMapScale[sPriKey]
	if self.tBucket[iScale] then
		self.tBucket[iScale][sPriKey] = nil
	end
end

function CTimingWheel.__helperFunc(self)
	--print("======CTimingWheel.__helperFunc=========")
	self.uTimerId = 0 --标志尚未启动定时器
	local iScale = self.iScale
	self.iScale = self.iScale + 1
	if self.iScale >= self.iScaleAmount then
		self.iScale = 1
	end

	--print(iScale)
	--print_r(self.tBucket)
	--print_r(self.tBucket[iScale])
	if self.tBucket[iScale] then
		local tBucket = self.tBucket[iScale]
		self.tBucket[iScale] = nil
		for sPriKey, func in pairs(tBucket) do
			self.dKeyMapScale[sPriKey] = nil
			func()
		end
	end

	if table.count(self.tBucket) > 0 and self.uTimerId==0 then--启动下一次定时器
		--print("=======callllater=======")
		self.uTimerId = timer.CallLater(handler(self, self.__helperFunc), self.iInterval, None, timer.NOT_REPEAT, timer.NO_NAME)
	end
end


--=================
--test
local goTest_timingWheel = nil
local gtTestObj = {}
local gi_test_id = 0

function callback_test()
	print("=========callback_test=========")
end

local test_class = class()
function test_class.__init__(self)
	gi_test_id = gi_test_id + 1
	self.test_id = gi_test_id
	self.count = 0
	goTest_timingWheel:addCallback(handler(self, self.callback), self.test_id)

end

function test_class.callback(self)
	--print("\n")
	print("=======test_class.callback====== self.test_id = " .. self.test_id)
	--print("goTest_timingWheel callbackAmount = " .. goTest_timingWheel:callbackAmount())
	--print("goTest_timingWheel hasCallback")
	--print(goTest_timingWheel:hasCallback(self.test_id))
	--print("\n")
	-- self.count = self.count + 1
	-- if self.count >= 3 then
	-- 	goTest_timingWheel:removeCallback(self.test_id)
	-- end
	goTest_timingWheel:addCallback(handler(self, self.callback), self.test_id)
end

function test_timingWheel()
	goTest_timingWheel = CTimingWheel(10, 10*1000)
	for i=1,10 do
		local obj = test_class()
		gtTestObj[i] = obj
	end
end



--test_timingWheel()
-------------------
