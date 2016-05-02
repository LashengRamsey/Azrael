
--[[
u.lua表示util.lua或utility.lua,工具模块
--]]
module('u', package.seeall)

None={} --表示没有

function error(...)
end

function printBytes(sBinary)
	sData=''
	for k,v in pairs({string.byte(sBinary,1,#sBinary)}) do
		sData=sData .. ' ' .. tostring(v)
	end
	print(string.format('printBytes,len=%s,data=%s',#sBinary,sData))
end

function checkInstance(obj)--检查是不是一个实例
	if type(obj)~='table' and type(obj)~='userdata' then
		error(string.format('不是一个实例,有可能调用函数时你用了点,而不是冒号.总之参数错了'),0)
	end
end

function err(sText,...)--包装一下,少敲键盘
	error(string.format(sText,...),0)
end

function reRaise(tError,sErrText)--重新抛出异常
	if type(tError)~='table' then
		error('参数tError必须是table,只能对xpcall返回的table进行再次抛出',0)
	end
	tError[1]=sErrText .. ';' .. tError[1] --加上更为高层的错误信息
	error(tError,0)
end

function setDefault(t,key,newValue)
	local oldVal=t[key]
	if oldVal~=nil then
		return oldVal
	end
	t[key]=newValue
	return newValue
end

function weakRef(obj)
	local function getFunc(tWeak)
		return tWeak.__obj__
	end
	return setmetatable({__obj__=obj},{__mode='v',__call=getFunc})
end

function proxy(obj)
	local function getFunc(tWeak,key)
		if tWeak.__obj__==nil then
			error('weakly-referenced object no longer exists',0)
		end
		return tWeak.__obj__[key]
	end
	local function setFunc(tWeak,key,value)
		if tWeak.__obj__==nil then
			error('weakly-referenced object no longer exists',0)
		end
		tWeak.__obj__[key]=value
	end
	return setmetatable({__obj__=obj},{__mode='v',__index=getFunc,__newindex=setFunc})
end

function errorHandler(uError)
	if type(uError)=='string' then
		--要判断出是不是系统抛出的,去掉行号 ,文件 C:\Users\yeWeiLong\Desktop\error.lua:6: attempt to index local 'i' (a number value)
		local iLevel
		if true then --系统检测到的错误
			iLevel=2
		else --用户主动调用error抛出的
			iLevel=3
		end
		local sTraceBack = debug.traceback('',iLevel)
		return {uError,sTraceBack}  --向外返回调用栈,出了本函数,外面的xpcall就无法拿到调用栈了
	elseif type(uError)=='table' then --中间路径xpcall得到的再次抛出的
		return uError
	else --一般是error函数第一个参数填错了,比如填了nil
		local sTraceBack = debug.traceback('',3)
		return {tostring(uError),sTraceBack}
	end
end

function mergeTuple(...)
	local tResult={}
	local iIndex=1
	for i,t in pairs({...}) do
		for k,v in pairs(t) do
			if type(k)=='number' then
				tResult[iIndex]=v
				iIndex=iIndex+1
			else
				error('合并的是元组,不是kv字典表,不可能来这里的',0)
				--tResult[k]=v
			end
		end
	end
	return tResult
end

DEAD={} --表示对象已释放

--闭包成员函数
function memFunc(func,obj,... )--不会影响实例的生命期
	if type(func)~='function' then
		error('func参数一定是个function',0)
	end
	if type(obj)~='table' and type(obj)~='userdata' then
		error('一定要传个实例过来',0)
	end
	local tag = debug.traceback()
	local tag2 = tostring(obj)
	local t1={...}
	local wr=weakRef(obj)--闭包weakRef对象,避免闭包obj本身,免得意外延长obj的生命期
	return function(...)
		local obj=wr() --进行提升
		if obj~=nil then--对象还活着,才进行调用
			local t2={...}
			local t3=mergeTuple(t1,t2)
			return func(obj, unpack(t3))
		else
			ZFM_LOG(PRINT_CIRITICAL, string.format('U.memFunc:该实例对象%s已经释放, 注册路径：', tag2), tag)
			return DEAD
		end
	end
end

function functor(func,...)
	if type(func)~='function' then
		error('func参数一定是个function',0)
	end
	local t1={...}
	return function(...)
		local t2={...}
		local t3=mergeTuple(t2,t1)

		return func(unpack(t3))
		--如果想中途print结果值,需要用下面的语法
		-- local tResult={func(unpack(t3))}
		-- print (tResult)
		-- return unpack(tResult)
	end
end



