

C_Timer = Timer 				--c层定时器
module("timer", package.seeall)

REPEAT = 1
NOT_REPEAT = 0
NO_NAME = "NO_NAME"

local _timerHandleId = 0		--定时器ID
local MAX_HANDLE_ID = 2^31-1
local _timerList = {}

--添加定时器
--callback回调函数
--delay间隔
--args回调函数
--cycle是否执行多次 0:只执行一次，1： 执行多次
--返回定时器ID，执行多次要手动删除定时器
function CallLater(callback, delay, args, cycle, name)
	--print("======CallLater===============")
	if _timerHandleId >= MAX_HANDLE_ID then
		_timerHandleId = 0
	end
	_timerHandleId = _timerHandleId + 1
	cycle = cycle or NOT_REPEAT
	name = name or NO_NAME
	local _timer = {callback = callback, delay = delay, id = _timerHandleId, args=args, cycle = cycle, name = name}
	table.insert(_timerList, _timer)
	C_Timer.addTimer(0, delay, _timerHandleId)
	return _timerHandleId
end

--删除定时器
function RemoveCallLater(id)
	C_Timer.delTimer(id)
	for k,v in ipairs(_timerList) do
		if v.id == id then
			table.remove(_timerList, k)
			break
		end
	end
end

--返回值，底层要用到
--返回0表示删除这个定时器，
--返回1不删除
function DoTimer(id)
	--print("==========DoTimer========== id =" .. id)
	for k,v in ipairs(_timerList) do
		if v.id == id then
			local ret = v.callback(v.args)
			if ret == 0 or v.cycle == 0 then	--只执行一次
 				table.remove(_timerList, k)
 				return 0
 			else
 				return 1
 			end
		end
	end
	return 0
end

--===========================
CTimerMng = class()

function CTimerMng.__init__(self)
	
end







