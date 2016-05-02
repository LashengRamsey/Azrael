
--作者:gooner
--关健时间点的事件
--整点事件
--整天事件
--整周事件
--整月事件

local gOnce
local gTempTime
local geNewHour
local geNewDay
local geNewWeek
local geNewMonth
local geNewMinu
local gDayNo
local gWeekNo
local gMonthNo

function onNewHour()
	if gTempTime then
		seconds = gTempTime
	else
		seconds = timeU.GetSecond()
	end

	local t=os.date("*t", i)
	geNewHour(t.year, t.month, t.day, t.hour, t.wday+1)--触发事件

	--各事件参数为,年,月,日,时,星期几(1~7,星期日是7,因为我已经加了1,原本是0~6的)
	--日期变化，触发日事件
	if gDayNo ~= timeU.getDayNo(seconds) then
		gDayNo = timeU.getDayNo(seconds)
		geNewDay(t.year, t.month, t.day, t.hour, t.wday+1)--触发事件
	end

	--周号变化，触发周事件
	if gWeekNo ~= timeU.getWeekNo(seconds) then
		gWeekNo = timeU.getWeekNo(seconds)
		geNewWeek(t.year, t.month, t.day, t.hour, t.wday+1)--触发事件
	end

	--月份变化，触发月事件
	if gMonthNo ~= timeU.getMonthNo(seconds) then
		gMonthNo = timeU.getMonthNo(seconds)
		geNewMonth(t.year, t.month, t.day, t.hour, t.wday+1)--触发事件
	end
end

function onNewMinum()
	geNewMinu()
end
	


function initTimerEvent()
	if gOnce then
		return
	end
	gOnce = true
	geNewHour = u.CEvent()
	geNewDay = u.CEvent()
	geNewWeek = u.CEvent()
	geNewMonth = u.CEvent()
	geNewMinu = u.CEvent()

	gDayNo = timeU.getDayNo()
	gWeekNo = timeU.getWeekNo()
	gMonthNo = timeU.getMonthNo()
	gTempTime = 0 -- 临时时间，用于指定时间的测试

	timer.gTimerMng.run(onNewHour, timeU.howManySecondNextHour(), 3600, 'onNewHour', nil, timer.LOWEST)--启动定时器
	timer.gTimerMng.run(onNewMinum, 0, 60, 'onNewMinu', nil, timer.LOWEST)
end

--initTimerEvent()

