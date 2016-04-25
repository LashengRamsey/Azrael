
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

local function searchAttr(sAttrName,clses)--是深度搜索
	for i,cls in ipairs(clses) do
		local attr = cls[sAttrName]--这里有多态行为,可能拿到的是父类的属性
		if attr then
			return attr
		end
	end
	return nil
end
local tInstanceMapClass=setmetatable({},{__mode='k'}) --实例映射class

local function setMetaTable(oInstance,cls)
	local tOldMetaTable=getmetatable(oInstance)
	if tOldMetaTable==nil then
		setmetatable(oInstance, cls) --直接把cls设为实例的元表,设这个{__index=cls}为实例的元表也行,就是其他的元方法不会工作
		return
	end
	local __index=tOldMetaTable.__index
	if __index==nil then
		error('作为一个别人的元表,竟然没有__index属性',0)
	end
	tInstanceMapClass[oInstance]=cls --不用担心实例的生命期被延长,因为是弱表
	if rawget(tOldMetaTable,'__bChangeIndex__') then --说明__index已经被动过手脚
		return
	end
	local func
	if type(__index)=='function' then --引擎创建的实例的元表的__index竟然是一个function,脚本创建的实例的元表肯定是一个table
		func=function(t,key)
			local cls=tInstanceMapClass[t]
			if cls~=nil then
				local attr=cls[key]--一定要优先搜索cls,然后才是原来的类,因为cls可能override了方法,
				if attr then
					return attr
				end
			end
			return __index(t,key) --在用原来的__index函数找
		end
	elseif type(__index)=='table' then
		func=function(t,key)
			local cls=tInstanceMapClass[t]
			if cls~=nil then
				return searchAttr(key,{cls,__index})--注意cls与__index的顺序,一定要优先搜索cls,然后才是原来的类
			else
				return __index[key]
			end
		end
	else
		error(string.format('元表的__index只能是function或table,不能是%s',type(__index)),0)
	end
	rawset(tOldMetaTable,'__bChangeIndex__',true) --基类竟然给元表设了元表
	tOldMetaTable.__index=func --引擎的类产生的每个实例是指向同一个metatable的,这里危险
	--setmetatable(oInstance, {__index=func})--不能这么做,因为有可能丢掉原来元表上已有的属性
	--另外,如果实例oInstance是引擎创建的,则oInstance是一个userdata,setmetatable的第一个参数必面是table,就是根本无法做到
end

-- function hasAttr(obj, key)
-- 	pcall(
-- 		)
-- end

local function __indexOfInstance(self, key)
	local cls=self.__class__
	-- if cls==nil then
	-- 	return nil
	-- end
	local attr=cls[key]

	-- if attr==nil then --实例中没有找到,类中也没有找到
	-- 	error(string.format("AttributeError: %s instance has no attribute '%s'",tostring(self),key),0)
	-- end

	-- 有些模块是类,却没有严格遵守类的做法,所以不敢打开注释,比如从udioEngine.lua继承的audioManager.lua
	-- 用于检查函数调用经常把该用冒号错写成点的情况
	if attr~=nil and type(attr)=='function' then
		return function(self,...)
			if type(self)~='table' and type(self)~='userdata' then
				error(string.format('不是一个实例,有可能调用函数时你用了点,而不是冒号.总之参数错了'))
			end
			return attr(self,...)
		end
	end
	return attr
end

local function __newIndexOfInstance(self,k,v)
	rawset(self,k,v) --用rawset避免死递归	
end

local function newInstance(cls,...)--生成类的实例
	if not rawget(cls,'__isClass__') then
		error('cls竟然不是一个类',0)
	end
	local self
	local __new__=cls.__new__ --获取cls.__new__有多态行为
	if __new__ then
		self=__new__(cls,...)
		if type(self)~='table' and type(self)~='userdata' then
			error(string.format('__new__函数必须返回一个table或userdata,你返回的是%s',type(self)),0)
		end
	else --已经不可能进这里了,因为全部类的父类object有__new__方法
		self={}
	end
	setMetaTable(self,cls)
	self.__class__=cls --标识这个实例所对应的类
	local __init__=cls.__init__ --获取cls.__init__有多态行为
	if __init__ then
		__init__(self,...)
	end
	return self
end

--是全部类的父类
cObject=setmetatable({},{__call=newInstance})
cObject.__bases__={}
cObject.__isClass__=true
cObject.__index=__indexOfInstance
cObject.__newindex=__newIndexOfInstance
cObject.__new__=function (cls,...) return {} end

function class(...)--创建一个class
	local arg={...}--lua 5.2 之后局部空间内不再自动生成arg,所以要手工生成一个
	for i,super in ipairs(arg) do
		if super[".isclass"]~=nil then
			error(string.format('要想从引擎继承%s,请自行实现__new__方法',tostring(super)),0)
		end
	end
	local cls=nil
	local iParentCount=table.getn(arg)
	if iParentCount==0 then--是基类,没有父类
		cls=setmetatable({},{__call=newInstance,__index=cObject})
	elseif iParentCount==1 then--是子类,有一个父类
		cls=setmetatable({}, {__call=newInstance,__index=arg[1]})
	else--是子类,有多个父类
		cls=setmetatable({}, {__call=newInstance,__index=function(t, key)return searchAttr(key,arg)end})
	end
	cls.__bases__={...} --保存这个属性只是为了用于判断类与类,实例与类之间的关系,暂时没有用上

	cls.__newindex=__newIndexOfInstance
	cls.__index=__indexOfInstance -- cls.__index=cls
	cls.__isClass__=true --方便检查一个表是不是类,获取时要用rawget,否则实例也会返回true
	return cls
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



-- cEvent=class()
-- 	function cEvent.__init__(self)
-- 			self.tHandler = {}
--				self.iId = 0
-- 	end
-- 	function cEvent.addEventHandler(self,func,sTag)
-- 		local tag
-- 		if sTag~=nil then
-- 			if type(sTag)~='string' then
-- 				error('事件响应的tag必须是string,你可以不提供,函数会返回一个标识给你')
-- 			end
-- 			tag=sTag
-- 		else
-- 			self.iId=self.iId+1
-- 			tag=self.iId
-- 		end
-- 		self.tHandler[tag]=func
-- 		return tag
-- 	end

-- 	function cEvent.removeEventHandler(self,tag)
-- 		self.tHandler[tag]=nil
-- 	end

-- 	function cEvent.triggerEvent(self,...)
-- 		for key,func in pairs(self.tHandler) do --不能用ipair,因为表中间有nil存在
-- 			local bRet=func(...)
-- 			if bRet==true then break end --如果其中一个事件响应函数返回true,则中断事件分发,即是之后的事件响应函数不会得到调用
-- 		end
-- 	end



--最基本的用法
--[[
local cAnimal=class()--生成一个类
	function cAnimal.__init__(self,sName,iAge)
		print('cAnimal.__init__')
		self.sName=sName
		self.iAge=iAge
	end

	function cAnimal.voice(self)
		print('base class,no voice',self.sName,self.iAge)
	end

local oAnimal=cAnimal('wang cai',7)--产生实例
oAnimal:voice()

oAnimal()

print('---------------------------')
--继承玩法----------------------

local cDog=class(cAnimal)

	function cDog.__init__(self,sName,iAge,iColor)--override
		print('cDog.__init__')
		self.iColor=iColor
		cAnimal.__init__(self,sName,iAge,iColor)--调用父类的同名方法

	end
	function cDog.voice(self) --override 重写父类方法
		cAnimal.voice(self) --调用父类的同名方法
		print('dog:wang wang')
	end
local oDog=cDog('wang cai',7,1)
oDog:voice()


oDog()
print('---------------------------')
--------------------------
----再次继承
local cPetDog=class(cDog)
	function cPetDog.voice(self)
		cDog.voice(self) --调用父类的同名方法
		print('cPetDog:miao miao')
	end
local oPetDog=cPetDog('wang cai',2,3)
oPetDog:voice()

print('---------------------------')
---以上都是单继承,接下来是多继承-------------------------

local cA=class()
	function cA.test(self)
		print('cA.test')
	end

	function cA.test_for_a(self)
		print('cA.test_for_a')
	end


local cB=class()
	function cB.test(self)
		print('cB.test')
	end
	function cB.test_for_b(self)
		print('cB.test_for_b')
	end


local cTest=class(cA,cB)
	function cTest.test(self) --override,这个必须重写,不然只会调用到其中一个父类的test方法
		cA.test(self) --调用父类的同名方法
		cB.test(self) --调用父类的同名方法
		print('cTest.test')
	end

-- local oTest=cTest()
-- oTest:test()
-- oTest:test_for_a()
-- oTest:test_for_b()


local cMutil=class(cDog)
	function cMutil.__new__(cls)
		return cTest()
	end

	function cMutil.test_for_b(self)
		cB.test_for_b(self)
		print('cMutil.test_for_b')
	end

local oMutil=cMutil()

oMutil:test_for_b()

--]]


