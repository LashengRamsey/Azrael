


--表操作
--以数组的方式找一个空位
function table.insertEx(t, v)
    local index
    for _k, _v in ipairs do
        index = _k
    end
    --t[index+1]肯定是空的
    t[index+1] = v
end

function table.removeEx(t, v)
    for _k, _v in pairs(t) do
        if _v == v then
            t[_k] = nil
        end
    end
end

--获取表中值
function table.get(t, k, default)
    if not k then
        return default
    end
    return t[k] or default
end

--返回指定表格中的所有键
-- 用法示例：
-- local t = {a = 1, b = 2, c = 3}
-- local keys = table.keys(t)
-- keys = {"a", "b", "c"}
function table.keys(t)
    local keys = {}
    if t == nil then
        return keys
    end
    for k, v in pairs(t) do
        keys[#keys + 1] = k
    end
    return keys
end


-- 返回指定表格中的所有值。
-- 格式：
-- values = table.values(表格对象)
-- 用法示例：
-- local t = {a = "1", b = "2", c = "3"}
-- local values = table.values(t)
-- -- values = {1, 2, 3}
function table.values(t)
    local values = {}
    if t == nil then
        return values
    end
    for k, v in pairs(t) do
        values[#values + 1] = v
    end
    return values
end

function table.isEmpty(t)
    return _G.next(t) == nil
end

--table t是否存在key
function table.has_key( t, key )
    for k, v in pairs(t) do
        if key == k then
            return true
        end
    end
    return false;
end

--table t是否存在value
function table.containValue( t, value )
    for k, v in pairs(t) do
        if value == v then
            return true
        end
    end
    return false;
end

function table.getKeyByValue( t, value )
    for k, v in pairs(t) do
        if value == v then
            return k
        end
    end
end

--table 将t2 的keys复制到t1中
function table.ikeys(t1, t2)
	for k,v in ipairs(t2) do
		table.insert(t1, k)
	end
end

--table 将t2 的keys复制到t1中
function table.kkeys(t1, t2)
	for k,v in pairs(t2) do
		table.insert(t1, k)
	end
end

function table.count(t)
	local i = 0
	for _,_ in pairs(t) do
		i = i + 1
	end
	return i
end

function table.nums(t)
    local i = 0
    for _,_ in pairs(t) do
        i = i + 1
    end
    return i
end

function table.isArray(t)
	if #t == table.count(t) then
		return true
	end
	return false
end

-- 合并两个表格。
-- 格式：
-- table.merge(目标表格对象, 来源表格对象)
-- 将来源表格中所有键及其值复制到目标表格对象中，如果存在同名键，则覆盖其值
function table.merge(dest, src)
    for k, v in pairs(src) do
        dest[k] = v
    end
end

--支持带环的表，拷贝出来是临时表
function table.deepCopy(object)
     local lookup_table = {}
     local function _copy(object)
         if type(object) ~= "table" then
            return object
         elseif lookup_table[object] then
            return lookup_table[object]
         end
         local new_table = {}
         lookup_table[object] = new_table
         for index, value in pairs(object) do
            new_table[_copy(index)] = _copy(value)
         end
         return setmetatable(new_table, getmetatable(object))--保留常量特性
         -- setmetatable(new_table,nil)
         -- return new_table
     end
     return _copy(object)
end

function table.remove2(t, v)
    for k,v1 in pairs(t) do
        if  type(v1) == type(v) and v1 == v then
            if type(k) == 'number' and k <= #t then
                table.remove(t, k)
            else
                t[k] = nil
            end
        end
    end
end

--连接tbale t1和t2，t1存在的字段以t2中的为主（t1和t2中的相同字段的value同为为table类型则再次连接），t1不存在的字段以t2的补全t1
function table.connect(t1, t2)
    if 'table' ~= type(t1) or 'table' ~= type(t2) then
        --ZFM_LOG(PRINT_CRITICAL,'警告：尝试使用table.connect连接非table类型')
        return false
    end
    for k2,v2 in pairs(t2) do
        if nil == t1[k2] then
            if type(k2) == 'number' then
                table.insert(t1, v2)
            else
                t1[k2] = v2
            end
        elseif 'table' == type(t1[k2]) and 'table' == type(v2) then
            table.connect(t1[k2], v2)
        else
            t1[k2] = v2
        end
    end
    return true
end

function table.reverse(v)
    local len = table.nums(v)
    if len == 1 then 
        return 
    end
    for i=2,len do
        table.insert(v,v[len-(i-1)])
        table.remove(v,len-(i-1))
    end

end

function table.join(t1,t2)
    if t1 == nil or t2 == nil then 
        error("不能连接数组为空")
    else
        for k,v in pairs(t2) do
            table.insert(t1,v)
        end
    end
end

function arrayContain( array, value)
    for i=1,#array do
        if array[i] == value then
            return true;
        end
    end
    return false;
end


--====================================
--字符串操作


-- 分割字符串。
-- 格式：
-- result = string.split(要分割的字符串, 分隔符)
-- 用法示例：
-- local result = string.split("1,2,3", ",")
-- result = {"1", "2", "3"}
function string.split(str, delimiter)
    if (delimiter=='') then return false end
    local pos,arr = 0, {}
    
    for st,sp in function() return string.find(str, delimiter, pos, true) end do
        table.insert(arr, string.sub(str, pos, st - 1))
        pos = sp + 1
    end
    table.insert(arr, string.sub(str, pos))
    return arr
end

--计算一个 UTF8 字符串包含的字符数量
--当一个 UTF8 字符串中包含中文时，string.len() 返回的结果是字符串的字节长度。string.utf8len() 会确保始终返回字符串中包含的字符数量。
function string.utf8len(str)
    local len  = #str
    local left = len
    local cnt  = 0
    local arr  = {0, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc}
    while left ~= 0 do
        local tmp = string.byte(str, -left)
        local i   = #arr
        while arr[i] do
            if tmp >= arr[i] then
                left = left - i
                break
            end
            i = i - 1
        end
        cnt = cnt + 1
    end
    return cnt
end

local function urlencodeChar(char)
    return "%" .. string.format("%02X", string.byte(c))
end

--为了通过 URL 传递数据，字符串中所有的非字母和数字都会被替换为“%编码”格式，空格会被替换为“+”。
function string.urlencode(str)
    -- convert line endings
    str = string.gsub(tostring(str), "\n", "\r\n")
    -- escape all characters but alphanumeric, '.' and '-'
    str = string.gsub(str, "([^%w%.%- ])", urlencodeChar)
    -- convert spaces to "+" symbols
    return string.gsub(str, " ", "+")
end

--将数字格式化为千分位格式。
--用法示例：
--local result = string.formatNumberThousands(12345)
-- result = "12,345"
function string.formatNumberThousands(num)
    local formatted = tostring(tonumber(num))
    local k
    while true do
        formatted, k = string.gsub(formatted, "^(-?%d+)(%d%d%d)", '%1,%2')
        if k == 0 then break end
    end
    return formatted
end

-- 删除字符串前部的空白字符。
-- 格式：
-- result = string.ltrim(字符串)
-- 空白字符包括：空格、制表符“\t”、换行符“\n”和“\r”。
-- 用法示例：
-- local result = string.ltrim("   \n\tHello")
-- result = "Hello"
function string.ltrim(str)
    return string.gsub(str, "^[ \t\n\r]+", "")
end

--删除字符串尾部的空白字符。
function string.rtrim(str)
    return string.gsub(str, "[ \t\n\r]+$", "")
end

--删除字符串两端的空白字符
function string.trim(str)
    str = string.gsub(str, "^[ \t\n\r]+", "")
    return string.gsub(str, "[ \t\n\r]+$", "")
end

-- 返回首字母大写的字符串
function string.ucfirst(str)
    return string.upper(string.sub(str, 1, 1)) .. string.sub(str, 2)
end

------------------------
--io

function io.exists(path)
    local file = io.open(path, "r")
    if file then
        io.close(file)
        return true
    end
    return false
end

function io.readfile(path)
    local file = io.open(path, "r")
    if file then
        local content = file:read("*a")
        io.close(file)
        return content
    end
    return nil
end

function io.writefile(path, content, mode)
    mode = mode or "w+b"
    local file = io.open(path, mode)
    if file then
        print("file is ok ok ok  ok ok ")
        if file:write(content) == nil then print("file is bad bad bad bad ") return false end
        io.close(file)
        return true
    else
        return false
    end
end

function io.pathinfo(path)
    local pos = string.len(path)
    local extpos = pos + 1
    while pos > 0 do
        local b = string.byte(path, pos)
        if b == 46 then -- 46 = char "."
            extpos = pos
        elseif b == 47 then -- 47 = char "/"
            break
        end
        pos = pos - 1
    end

    local dirname = string.sub(path, 1, pos)
    local filename = string.sub(path, pos + 1)
    extpos = extpos - pos
    local basename = string.sub(filename, 1, extpos - 1)
    local extname = string.sub(filename, extpos)
    return {
        dirname = dirname,
        filename = filename,
        basename = basename,
        extname = extname
    }
end

function io.filesize(path)
    local size = false
    local file = io.open(path, "r")
    if file then
        local current = file:seek()
        size = file:seek("end")
        file:seek("set", current)
        io.close(file)
    end
    return size
end

function math.newrandomseed()
    local ok, socket = pcall(function()
        return require("socket")
    end)

    if ok then
        math.randomseed(socket.gettime() * 1000)
    else
        math.randomseed(os.time())
    end
    math.random()
    math.random()
    math.random()
    math.random()
end

function math.round(value)
    value = checknumber(value)
    return math.floor(value + 0.5)
end

local pi_div_180 = math.pi / 180
function math.angle2radian(angle)
    return angle * pi_div_180
end

local pi_mul_180 = math.pi * 180
function math.radian2angle(radian)
    return radian / pi_mul_180
end

