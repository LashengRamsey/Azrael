--序列化一个Table  
function tableToStr(t)  
    local mark={}  
    local assign={}  
  
    local function table2str(t, parent)  
        mark[t] = parent  
        local ret = {}  
  
        if table.isArray(t) then  
            for i,v in pairs(t) do  
                local k = tostring(i)  
                local dotkey = parent.."["..k.."]"  
                local t = type(v)  
                if t == "userdata" or t == "function" or t == "thread" or t == "proto" or t == "upval" then  
                    --ignore  
                elseif t == "table" then  
                    if mark[v] then  
                        table.insert(assign, dotkey.."="..mark[v])  
                    else  
                        table.insert(ret, table2str(v, dotkey))  
                    end  
                elseif t == "string" then  
                    table.insert(ret, string.format("%q", v))  
                elseif t == "number" then  
                    if v == math.huge then  
                        table.insert(ret, "math.huge")  
                    elseif v == -math.huge then  
                        table.insert(ret, "-math.huge")  
                    else  
                        table.insert(ret,  tostring(v))  
                    end  
                else  
                    table.insert(ret,  tostring(v))  
                end  
            end  
        else  
            for f,v in pairs(t) do  
                local k = type(f)=="number" and "["..f.."]" or f  
                local dotkey = parent..(type(f)=="number" and k or "."..k)  
                local t = type(v)  
                if t == "userdata" or t == "function" or t == "thread" or t == "proto" or t == "upval" then  
                    --ignore  
                elseif t == "table" then  
                    if mark[v] then  
                        table.insert(assign, dotkey.."="..mark[v])  
                    else  
                        table.insert(ret, string.format("%s=%s", k, table2str(v, dotkey)))  
                    end  
                elseif t == "string" then  
                    table.insert(ret, string.format("%s=%q", k, v))  
                elseif t == "number" then  
                    if v == math.huge then  
                        table.insert(ret, string.format("%s=%s", k, "math.huge"))  
                    elseif v == -math.huge then  
                        table.insert(ret, string.format("%s=%s", k, "-math.huge"))  
                    else  
                        table.insert(ret, string.format("%s=%s", k, tostring(v)))  
                    end  
                else  
                    table.insert(ret, string.format("%s=%s", k, tostring(v)))  
                end  
            end  
        end  
  
        return "{"..table.concat(ret,",").."}"  
    end  
  
    if type(t) == "table" then  
        return string.format("%s%s",  table2str(t,"_"), table.concat(assign," "))  
    else  
        return tostring(t)  
    end  
end  


--反序列化一个Table  
function strToTable(str)
	local EMPTY_TABLE = {} 
    if str == nil or str == "nil" then  
        return {}  
    elseif type(str) ~= "string" then  
        EMPTY_TABLE = {}  
        return EMPTY_TABLE  
    elseif #str == 0 then  
        EMPTY_TABLE = {}  
        return EMPTY_TABLE  
    end  
  
    local code, ret = pcall(loadstring(string.format("do local _=%s return _ end", str)))  
  
    if code then  
        return ret  
    else  
        EMPTY_TABLE = {}  
        return EMPTY_TABLE  
    end  
end


-- 打印整个table
function print_r(tb)
    if type(tb) ~= "table" then
        print(tb)
        return
    end
    local str = ""
    local tab = "    "
    local count = -1
    local function _print(t)
        count = count + 1
        local temp = ""
        local tmp = tab
        for i = 1, count do
            temp = string.format("%s%s", temp, tab)-- temp .. tab
            tmp = string.format("%s%s", tmp, tab)-- tmp .. tab
        end
        if type(t) ~= "table" then
            local data = tostring(t)
            if type(t) == "string" then
                data = string.format("\"%s\"", data)
            end
            str = string.format("%s%s,\n", str, data)
            count = count - 1
            return
        end

        str = string.format("%s\n%s{\n", str, temp) --str .. "\n" .. temp .. "{" .. "\n"
        for i, v in pairs(t) do
            local key = tostring(i)
            if type(i) == "string" then
                key = string.format("\"%s\"", key)
            end
            str = string.format("%s%s[%s] = ", str, tmp, key)--str .. tmp .. "[" .. i .. "]" .. "="
            _print(v)
        end
        str = string.format("%s%s},\n", str, temp) --str .. temp .. "}\n"
        count = count - 1
    end
    _print(tb)
    print(str)
end

function print_lua_table(lua_table, indent)
    indent = indent or 0
    for k, v in pairs(lua_table) do
        if type(k) == "string" then
            k = string.format("%q", k)
        end
        local szSuffix = ""
        if type(v) == "table" then
            szSuffix = "{"
        end
        local szPrefix = string.rep("    ", indent)
        formatting = szPrefix.."["..k.."]".." = "..szSuffix
        if type(v) == "table" then
            print(formatting)
            print_lua_table(v, indent + 1)
            print(szPrefix.."},")
        else
            local szValue = ""
            if type(v) == "string" then
                szValue = string.format("%q", v)
            else
                szValue = tostring(v)
            end
            print(formatting..szValue..",")
        end
    end
end


local tonumber_ = tonumber
function tonumber(v, base)
    return tonumber_(v, base) or 0
end

function toint(v)
    return math.round(tonumber(v))
end

function tobool(v)
    return (v ~= nil and v ~= false)
end

function totable(v)
    if type(v) ~= "table" then v = {} end
    return v
end

--复制对象
function clone(object)
    local lookup_table = {}
    local function _copy(object)
        if type(object) ~= "table" then
            return object
        elseif lookup_table[object] then
            return lookup_table[object]
        end
        local new_table = {}
        lookup_table[object] = new_table
        for key, value in pairs(object) do
            new_table[_copy(key)] = _copy(value)
        end
        return setmetatable(new_table, getmetatable(object))
    end
    return _copy(object)
end

function handler(target, method)
    return function(...) return method(target, ...) end
end


--计数器
local function newCounter()
    local i = 0
    return function()     -- anonymous function
       i = i + 1
        return i
    end
end

local g_id_generator = newCounter()
function getNextID()
  local nextID 
    nextID = g_id_generator()
    return nextID
end

--去除扩展名
function stripExtension(filename)
    local idx = filename:match(".+()%.%w+$")
    if(idx) then
        return filename:sub(1, idx-1)
    else
        return filename
    end
end

--获取路径
function stripfilename(filename)
    return string.match(filename, "(.+)/[^/]*%.%w+$") --*nix system
    --return string.match(filename, “(.+)\\[^\\]*%.%w+$”) — windows
end

--获取文件名
function strippath(filename)
    return string.match(filename, ".+/([^/]*%.%w+)$") -- *nix system
    --return string.match(filename, “.+\\([^\\]*%.%w+)$”) — *nix system
end

CEvent=class()
function CEvent.__init__(self)
    self.tHandler = {}
    self.iId = 0
end
function CEvent.addEventHandler(self,func,sTag)
    local tag
    if sTag~=nil then
        if type(sTag)~='string' then
            error('事件响应的tag必须是string,你可以不提供,函数会返回一个标识给你')
        end
        tag=sTag
    else
        self.iId=self.iId+1
        tag=self.iId
    end
    self.tHandler[tag]=func
    return tag
end

function CEvent.removeEventHandler(self,tag)
    self.tHandler[tag]=nil
end

function CEvent.triggerEvent(self,...)
    for key,func in pairs(self.tHandler) do --不能用ipair,因为表中间有nil存在
        local bRet=func(...)
        if bRet==true then break end --如果其中一个事件响应函数返回true,则中断事件分发,即是之后的事件响应函数不会得到调用
    end
end

function isInstance(obj,cls)--判断一个实例是不是某个类的对象(有面向对象语义,奶牛也是牛)
    if obj.__class__==nil then
        error('obj 不是实列',0)
    end
    if not cls.__isClass__ then
        error('cls 不是类',0)
    end
    return isSubClass(obj.__class__,cls)
end

function isSubClass(cls,superCls)--判断1个类是否为另1个类的子类
    if not cls.__isClass__ then
        error('cls 不是类',0)
    end
    if not superCls.__isClass__ then
        error('superCls 不是类',0)
    end

    for i,tempCls in ipairs(cls.__bases__) do
        if tempCls==superCls then
            return true     
        end
    end

    for i,tempCls in ipairs(cls.__bases__) do
        if isSubClass(tempCls,superCls) then
            return true
        end
    end
    return false
end

