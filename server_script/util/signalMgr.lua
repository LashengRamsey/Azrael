module('signalMgr', package.seeall)
-- 新手指引
---------------------------------------------------------------------

_G.Azrael = _G.Azrael or {}
local z = _G.Azrael
function z.event(name, _usedata,...)
    local event = {}
    event.name = name
    event._usedata = _usedata
    local t = {...}
    table.merge(event, t)
    return event
end


--[[
玩法：
    1.监听同样信号，且优先级一致的监听者，接收到信号的顺序是无序的
    2.使用zfm.event创建消息体时，第三个参数开始为自定义数据部分，通过索引（1开始）开始访问
    3.注册监听addEventListener(eventName, priority, bOnce, obj, func,  ...)
        eventName：字符串，不区分大小写，监听的信号; 
        priority：无符号整型，优先级, 默认为1; 
        bOnce：布尔型，是否仅监听一次， 默认为false; 
        obj:若func为成员函数则obj必须传对应的实例，否则传nil
        func:回调函数; 
        ...:为注册时的实参（将会在监听时回传）
    4.移除监听：removeEventListener(eventName, priority,  obj, func)        
    5.分派信号：dispatchEvent(event)，event可使用zfm.event(XXX,XXX)创建，或自行创建table（该table必须包含字段name作为信号量名）
    6.监听函数定义：
        local function callback(event,...)
            --body
            return true --true为截住信号，不再传播
        end
        '...'为注册时传的实参
    7.例子：
        定义信号管理器单例：
            reuqire '文件路径'
            gSignalMgr = signalMgr.CSignalMgr.new()
        定义回调：
        function callback(event,...)
            --body
        end
        注册监听：
        gSignalMgr:addEventListener('eventName',1, false, nil, callback)
        分派信号：
        gSignalMgr:dispatchEvent(zfm.event('eventName', 1, {sdf = 1}, l))
        移除监听：
        gSignalMgr:removeEventListener('eventName',1,  nil, callback)

]]

CSignalMgr = class()

function CSignalMgr.__init__(self)
    self.listeners_ = {}
    self.nextListenerHandleIndex_ = 0
    self.maxPriority = 1
end

function CSignalMgr.addEventListener(self, eventName, priority, bOnce, obj, func,  ...)
    assert(type(eventName) == "string" and eventName ~= "",
        "CSignalMgr.addEventListener() - invalid eventName")
    if priority and type(priority) ~= "number" or priority and priority <= 0 then
        error(string.format("CSignalMgr.addEventListener() - invalid priority "), 0)
    end
    if bOnce and type(bOnce) ~= "boolean"  then
        error(string.format("CSignalMgr.addEventListener() - invalid bOnce "), 0)
    end

    local t1={...}
    local listener = obj and u.memFunc(func, obj,...)
        or function (...) 
            t2 = {...}
            -- table.merge(t1, t2)
            return func(unpack(t2), t1) 
        end

    eventName = string.upper(eventName)
    if self.listeners_[eventName] == nil then
        self.listeners_[eventName] = {}
    end

    self.nextListenerHandleIndex_ = self.nextListenerHandleIndex_ + 1
    local handle = tostring(self.nextListenerHandleIndex_)
    local tag = string.format('%s%s',tostring(obj),tostring(func))

    priority = priority or 1
    self.maxPriority = math.max(self.maxPriority, priority)
    bOnce = bOnce or false
    if self.listeners_[eventName][priority] == nil then
        self.listeners_[eventName][priority] = {}
    end
    self.listeners_[eventName][priority][handle] = {listener, tag, priority, bOnce}
    -- table.sort(self.listeners_[eventName], function (A, B)
    --         return A.priority < B.priority
    --     end)
    if DEBUG > 1 then
        printInfo("%s [Event] addEventListener() - CSignalMgr. %s, handle: %s, tag: \"%s\"",
                  tostring(self), eventName, handle, tostring(tag))
    end

    return handle
end

function CSignalMgr.addOnce(self, eventName, priority,obj, func,  ...)
    return self:addEventListener(eventName, priority, true, obj, func,  ...)
end

function CSignalMgr.removeEventListener(self, eventName, priority,  obj, func)
    local tagToRemove = string.format('%s%s',tostring(obj),tostring(func))
    local priority = priority or 1
    -- local bOnce = bOnce or false
    eventName = string.upper(eventName)
    if self.listeners_[eventName] and self.listeners_[eventName][priority] then
        for handle, listener in pairs(self.listeners_[eventName][priority]) do
            if listener[2] == tagToRemove then
                self.listeners_[eventName][priority][handle] = nil
                if DEBUG > 1 then
                    printInfo("%s [Event] removeEventListener() - remove listener [%s] for event %s", tostring(self), handle, eventName)
                end
            end
        end
    end
end

function CSignalMgr.dispatchEvent(self, event)
    event.name = string.upper(tostring(event.name))
    local eventName = event.name
    if DEBUG > 1 then
        printInfo("%s [Event] dispatchEvent() - event %s", tostring(self), eventName)
    end

    if self.listeners_[eventName] == nil then return end
    -- event.target = self
    -- event.stop_ = false
    -- event.stop = function(self)
    --     self.stop_ = true
    -- end
    for i = 0, self.maxPriority do
        if self.listeners_[eventName][i] then
            for handle, listener in pairs(self.listeners_[eventName][i]) do
                if DEBUG > 1 then
                    printInfo("%s [Event] dispatchEvent() - dispatching event %s to listener %s", tostring(self), eventName, handle)
                end
                -- listener[1] = listener
                -- listener[2] = tag
                event.tag = listener[2]
                local reval = listener[1](event)
                if true == listener[4] then
                    self.listeners_[eventName][i][handle] = nil
                end
                if reval then
                    if DEBUG > 1 then
                        printInfo("%s [Event] dispatchEvent() - break dispatching for event %s", tostring(self), eventName)
                    end
                end
            end
        end
    end

end

function CSignalMgr.removeEventListenerByHandle(self, handleToRemove)
    for eventName, listenersForEvent in pairs(self.listeners_) do
        for i = 0, self.maxPriority do
            if listenersForEvent[i] then
                for handle, _ in pairs(listenersForEvent[i]) do
                    if handle == handleToRemove then
                        listenersForEvent[i][handle] = nil
                        if DEBUG > 1 then
                            printInfo("%s [Event] removeEventListener() - remove listener [%s] for event %s", tostring(self), handle, eventName)
                        end
                    end
                end
            end
        end
    end

end

function CSignalMgr.removeEventListenersByTag(self, tagToRemove)
    for eventName, listenersForEvent in pairs(self.listeners_) do
        for i = 0, self.maxPriority do
            if listenersForEvent[i] then
                for handle, listener in pairs(listenersForEvent[i]) do
                    if listener[2] == tagToRemove then
                        listenersForEvent[i][handle] = nil
                        if DEBUG > 1 then
                            printInfo("%s [Event] removeEventListener() - remove listener [%s] for event %s", tostring(self), handle, eventName)
                        end
                    end
                end
            end
        end
    end
end

function CSignalMgr.removeEventListenersByEvent(self, eventName)
    self.listeners_[string.upper(eventName)] = nil
    if DEBUG > 1 then
        printInfo("%s [Event] removeAllEventListenersForEvent() - remove all listeners for event %s", tostring(self), eventName)
    end
end

function CSignalMgr.removeAllEventListeners()
    self.listeners_ = {}
    if DEBUG > 1 then
        printInfo("%s [Event] removeAllEventListeners() - remove all listeners", tostring(self))
    end
end

function CSignalMgr.hasEventListener(self, eventName, priority)
    eventName = string.upper(tostring(eventName))
    local t = self.listeners_[eventName]
    local count = 0
    if t then
        for p, handle in pairs(t) do
            if priority then 
                if priority == p then
                    count = count + table.nums(handle)
                    break
                end
            else
                count = count + table.nums(handle)
            end
        end
    end
    return count ~= 0 and count or nil, priority and t and t[priority] or t
end

function CSignalMgr.dumpAllEventListeners(self)
    print("---- CSignalMgr.dumpAllEventListeners() ----")
    for name, listeners in pairs(self.listeners_) do
        printf("-- CSignalMgr. %s", name)
        for i = 0, self.maxPriority do
            if listeners[i] then                
                for handle, listener in pairs(listeners[i]) do
                    printf("--     listener: %s, handle: %s, priority: %d", tostring(listener[1]), tostring(handle), tonumber(i))
                end
            end
        end
    end
end

