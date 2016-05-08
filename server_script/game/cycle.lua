module("cycle", package.seeall)

--只维持n个周期后就会自动清理的数据
CCycleData = class(pst.CPersist)
--iKeepCyc	数据保留的时间周期
--cDirtyHandler	数据变化事件响应函数.
function CCycleData.__init__(self, iKeepCyc, CDirtyHandler)
	pst.CPersist.__init__(self, CDirtyHandler)
	if iKeepCyc < 1 then
		iKeepCyc = 1
	end
	self.iKeepCyc = iKeepCyc
	self.tData = {}
end

function CCycleData.save(self)--override
	return self.tData
end

function CCycleData.load(self, tData)--override
	local iCurCycNo = self:getCycleNo()
	local tCycNo = table.keys(tData)
	table.sort(tCycNo)
	--从最早的开始检查,可以尽快地退出循环
	local bIsDirty = false

	for _, iCycNo in ipairs(tCycNo) do
		if iCurCycNo >= iCycNo + self.iKeepCyc then
			tData[iCycNo] = nil
			bIsDirty = true
		else
			break
		end
	end
	if bIsDirty then
		self:markDirty()
	end
	self.tData = tData
end

function CCycleData.set(self, key, value)--override
	local iCycNo = self:getCycleNo()
	if not self.tData[iCycNo] then
		self.tData[iCycNo] = {}
	end
	self.tData[iCycNo][key] = value
	self:markDirty()
end

function CCycleData.delete(self, key)
	local iCycNo = self:getCycleNo()
	if not self.tData[iCycNo] or not self.tData[iCycNo][key] then
		return
	end
	self.tData[iCycNo][key] = nil
	self:markDirty()
end

function CCycleData.add(self, key, iValue, iDefault)--override	--返回成功后的结果值
	iDefault = iDefault or 0
	local iCycNo = self:getCycleNo()
	if not self.tData[iCycNo] then
		self.tData[iCycNo] = {}
	end
	self.tData[iCycNo][key] = iValue + (self.tData[iCycNo][key] or iDefault)
	self:markDirty()
	return self.tData[iCycNo][key]
end

--override
--iWhichCyc的值范围 0当前周期,-1上一个周期,-2上二个周期,-3上三个周期,以此类推
--如果是天记录,则相应是 0今天,-1昨天,-2前天,-3大前天,以此类推
function CCycleData.fetch(self, key, iDefault, iWhichCyc)
	iDefault = iDefault or 0
	iWhichCyc = iWhichCyc or 0
	if iWhichCyc > 0 then
		err('iWhichCyc值为%d,大于0是没有意义的', iWhichCyc)
	end
	local iCycNo = self:getCycleNo() + iWhichCyc
	return table.get(table.get(self.tData, iCycNo, {}), key, iDefault)
end

--清除某个周期的数据,这个很危险,基本上只是方便QC在内服做测试用的
function CCycleData.clear(self, iWhichCyc)	
	iWhichCyc = iWhichCyc or 0
	if iWhichCyc > 0 then
		err('iWhichCyc值为%d,大于0是没有意义的', iWhichCyc)
	end
	local iCycNo = self:getCycleNo() + iWhichCyc
	if self.tData[iCycNo] then
		self.tData[iCycNo] = nil
		self:markDirty()
	end
end

function CCycleData.getCycleNo(self)
	error('请在子类override')
end

--小时变量
CCycHour = class(CCycleData)
function CCycHour.getCycleNo(self)--override
	return timeU.GetHourNo()
end

--天变量
CCycDay = class(CCycleData)
function CCycDay.getCycleNo(self)--override
	return timeU.GetDayNo()
end

--周变量
CCycWeek = class(CCycleData)
function CCycWeek.getCycleNo(self)--override
	return timeU.GetWeekNo()
end

--月变量
CCycMonth = class(CCycleData)
function CCycMonth.getCycleNo(self)--override
	return timeU.GetMonthNo()
end



CThisTemp = class(pst.CPersist)
--[[
临时变量
--]]
function CThisTemp.__init__(self, CDirtyHandler)
	pst.CPersist.__init__(self, CDirtyHandler)
	self.dataList = {}
	self.timeList = {}
end
	
function CThisTemp.save(self)
	local data = {}
	data["dataList"] = self.dataList
	data["timeList"] = self.timeList
	return data
end
	
function CThisTemp.load(self, data)
	self.dataList = data["dataList"]
	self.timeList = data["timeList"]
	self.checkTimeout()
end
	
function CThisTemp.checkTimeout(self)
	local now = timeU.GetSecond()
	local keyList = table.keys(self.dataList)
	local updated = false
	for _, key in pairs(keyList) do
		if self.timeList[key] <= now then
			self.dataList[key] = nil
			self.timeList[key] = nil
			updated = true
		end
	end
	if updated then
		self:markDirty()
	end
end
	
function CThisTemp.set(self, key, val, ti)
	self.checkTimeout()
	self:markDirty()
	self.dataList[key] = val
	self.timeList[key] = timeU.GetSecond() + ti
end
	
function CThisTemp.add(self, key, val, ti)
	self.checkTimeout()
	self:markDirty()
	self.dataList[key] = self.dataList[key] or 0 + val
	if not self.timeList[key] then
		self.timeList[key] = getSecond() + ti
	end
	return self.dataList[key]
end
		
function CThisTemp.delete(self, key, default)
	default = default or 0
	if self.dataList[key] then
		self.timeList[key] = nil
		self.dataList[key] = nil
		self:markDirty()
	end
end

function CThisTemp.fetch(self, key, default)
	default = default or 0
	self.checkTimeout()
	return  self.dataList[key] or default
end

function CThisTemp.fetchTime(self, key, default)
	default = default or 0
	self.checkTimeout()
	return self.timeList[key] or default
end
	

---============================
--test

local CTestCycle = class()
function CTestCycle.__init__(self)
	self.cycHour = CCycHour(2)
	self.cycDay = CCycDay(2)
	print(self.cycDay)
	self.cycWeek = CCycWeek(2)
	self.cycMonth = CCycMonth(2)
	--self.cycTemp = CThisTemp()
end

function CTestCycle.test(self)
	print("=======CTestCycle==========")
	local iCurCycNo = self.cycDay:getCycleNo()
	self.cycDay:load({[iCurCycNo]={["bb"]=123, ["aa"]="adf"}})
	self.cycDay:fetch("bb", 0)
	self.cycDay:fetch("aa", "")
	self.cycDay:set("t", {["a"]="a", ["b"]="b", ["i"]=1})
	self.cycDay:set("s", "string")
	self.cycDay:set("i", 12)
	print("save")
	print_r(self.cycDay:save())
	print("save")
	self.cycDay:delete("i")
	print_r(self.cycDay:save())
	print("\n")
end

function test_cycle()
	local obj = CTestCycle()
	obj:test()
end
--test_cycle()

--------------------------------
