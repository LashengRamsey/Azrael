module("timeU", package.seeall)

--时间相关的工具函数

local gtStandardTime = {2015,11,16,0,0,0}
local giStandardTime = os.time({year=2015, month=11, day=16, hour=0, min=0, sec=0})

--从1970年1月1日8点至今经过的秒数
function GetSecond()
	return os.time()
end

--今天的起始时间戳，秒数
function todDayStartStamp(i)
	if not i then
		i = GetSecond()
	end
	local t = os.date("*t", i)
	----t.isdst是否是夏令时之类的
	return os.time({year=t.year, month=t.month, day=t.day, hour=0, min=0, sec=0, isdst=t.isdst})
end

--下一天距离现在多少秒
function howManySecondNextDay(i)
	if not i then
		i = GetSecond()
	end
	local t = os.date("*t", i)
	--直接对day加1,即使当前是月尾最后一天,也不会有问题
	----t.isdst是否是夏令时之类的
	return os.time({year=t.year, month=t.month, day=t.day+1, hour=0, min=0, sec=0, isdst=t.isdst}) - i
end

--下一小时距离现在多少秒
function howManySecondNextHour(i)
	if not i then
		i = GetSecond()
	end
	local t = os.date("*t", i)
	--直接对hour加1,即使当前是23点,也不会有问题
	return os.time({year=t.year, month=t.month, day=t.day, hour=t.hour+1, min=0, sec=0, isdst=t.isdst}) - i
end

--下一周距离现在多少秒
function howManySecondNextWeek(i)
	if not i then
		i = GetSecond()
	end
	local t = os.date("*t", i)
	local iDay=6-t.wday
	return os.time({year=t.year, month=t.month, day=t.day+1, hour=0, min=0, sec=0, isdst=t.isdst})-i+iDay*24*60*60
end

--下一月距离现在多少秒
function howManySecondNextMonth(i)
	if not i then
		i = GetSecond()
	end
	local t = os.date("*t", i)
	--直接对month加1,即使当前是12月,也不会有问题
	return os.time({year=t.year, month=t.month+1, day=1, hour=0, min=0, sec=0, isdst=t.isdst})-i
end

--时间戳,分钟序号,从1开始
function GetMinuteNo(i)
	if not i then
		i = GetSecond()
	end
	return math.floor((i-giStandardTime)/60+1)
end

--时间戳,小时序号,从1开始
function GetHourNo(i)
	if not i then
		i = GetSecond()
	end
	return math.floor((i-giStandardTime)/3600+1)
end

--时间戳,天序号,从1开始
function GetDayNo(i)
	if not i then
		i=GetSecond()
	end
	return math.floor((i-giStandardTime)/3600/24+1)
end

--时间戳,周序号,从1开始
function GetWeekNo(i)
	if not i then
		i=GetSecond()
	end
	return math.floor((i-giStandardTime)/3600/24/7+1)
end

--时间戳,月序号,从1开始
function GetMonthNo(i)
	if not i then
		i=GetSecond()
	end
	local tTime = os.date("*t", i)
	return math.floor((tTime.year - gtStandardTime[1]) * 12 + (tTime.month - gtStandardTime[2]) + 1)
end

--#当天是否是当前月的第一天
function isFirstDayFromMonth()  
	local t = os.date("*t", GetSecond())
	return t.day==1
end

function isNewMinute()
	local t = os.date("*t", GetSecond())
	return t.min==0
end

function isNewHour()
	local t = os.date("*t", GetSecond())
	return t.hour==0
end

function isNewDay(iHour)
	local t = os.date("*t", GetSecond())
	return t.day==0
end

--返回时间文本，such as  94天21时56分5秒
function getTimeStr(i)
	local iDay=i/(24*3600)
	local iHour=i%(24*3600)/3600
	local iMin=i%(3600*24)%3600/60
	local iSec=i%60

	local t = {}
	if iDay then
		table.insert(t, "" .. iDay .. "天")
	end
	if iHour then
		table.insert(t, "" .. iHour .. "小时")
	end
	if iMin then
		table.insert(t, "" .. iMin .. "分钟")
	end
	if iSec then
		table.insert(t, "" .. iSec .. "秒")
	end
	return table.concat(t)
end

--------------------
--test

function test_timeU()
	print("=========test_timeU============")
	print("=====================GetSecond() = " .. GetSecond())
	print("==============todDayStartStamp() = " .. todDayStartStamp())
	print("==========howManySecondNextDay() = " .. howManySecondNextDay())
	print("=========howManySecondNextHour() = " .. howManySecondNextHour())
	print("=========howManySecondNextWeek() = " .. howManySecondNextWeek())
	print("========howManySecondNextMonth() = " .. howManySecondNextMonth())
	print("===================GetMinuteNo() = " .. GetMinuteNo())
	print("=====================GetHourNo() = " .. GetHourNo())
	print("======================GetDayNo() = " .. GetDayNo())
	print("=====================GetWeekNo() = " .. GetWeekNo())
	print("====================GetMonthNo() = " .. GetMonthNo())
	print("===========isFirstDayFromMonth() = " .. ((isFirstDayFromMonth() and "true") or "false"))
	print("===================isNewMinute() = " .. ((isNewMinute() and "true") or "false"))
	print("=====================isNewHour() = " .. ((isNewHour() and "true") or "false"))
	print("======================isNewDay() = " .. ((isNewDay() and "true") or "false"))

	--测试hour直接加24
	local t = os.date("*t", i)
	print_r(t)
	local ni = os.time({year=t.year, month=t.month, day=t.day, hour=t.hour + 24, min=0, sec=0, isdst=t.isdst})
	print("os.time = " .. ni)
	print_r(os.date("*t", ni))
	print(getTimeStr(ni-GetSecond()))
end

--test_timeU()


