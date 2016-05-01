
local function searchAttr(sAttrName, clses)--是深度搜索
	for i, cls in ipairs(clses) do
		local attr = cls[sAttrName]--这里有多态行为,可能拿到的是父类的属性
		if attr then
			return attr
		end
	end
	return nil
end

local tInstanceMapClass=setmetatable({}, {__mode='k'}) --实例映射class
local function setMetaTable(oInstance,cls)
	local tOldMetaTable = getmetatable(oInstance)
	if tOldMetaTable == nil then
		setmetatable(oInstance, cls) --直接把cls设为实例的元表,设这个{__index=cls}为实例的元表也行,就是其他的元方法不会工作
		return
	end
	local __index = tOldMetaTable.__index
	if __index == nil then
		error('作为一个别人的元表,竟然没有__index属性',0)
	end
	tInstanceMapClass[oInstance] = cls --不用担心实例的生命期被延长,因为是弱表
	if rawget(tOldMetaTable, '__bChangeIndex__') then --说明__index已经被动过手脚
		return
	end
	local func
	if type(__index) == 'function' then --引擎创建的实例的元表的__index竟然是一个function,脚本创建的实例的元表肯定是一个table
		func=function(t,key)
			local cls = tInstanceMapClass[t]
			if cls~=nil then
				local attr = cls[key]--一定要优先搜索cls,然后才是原来的类,因为cls可能override了方法,
				if attr then
					return attr
				end
			end
			return __index(t,key) --在用原来的__index函数找
		end
	elseif type(__index) == 'table' then
		func=function(t,key)
			local cls = tInstanceMapClass[t]
			if cls ~= nil then
				return searchAttr(key, {cls, __index})--注意cls与__index的顺序,一定要优先搜索cls,然后才是原来的类
			else
				return __index[key]
			end
		end
	else
		error(string.format('元表的__index只能是function或table,不能是%s',type(__index)),0)
	end
	rawset(tOldMetaTable, '__bChangeIndex__', true) --基类竟然给元表设了元表
	tOldMetaTable.__index = func --引擎的类产生的每个实例是指向同一个metatable的,这里危险
	--setmetatable(oInstance, {__index=func})--不能这么做,因为有可能丢掉原来元表上已有的属性
	--另外,如果实例oInstance是引擎创建的,则oInstance是一个userdata,setmetatable的第一个参数必面是table,就是根本无法做到
end

local function __indexOfInstance(self, key)
	local cls = self.__class__
	-- if cls==nil then
	-- 	return nil
	-- end
	local attr = cls[key]

	-- if attr==nil then --实例中没有找到,类中也没有找到
	-- 	error(string.format("AttributeError: %s instance has no attribute '%s'",tostring(self),key),0)
	-- end

	-- 有些模块是类,却没有严格遵守类的做法,所以不敢打开注释,比如从udioEngine.lua继承的audioManager.lua
	-- 用于检查函数调用经常把该用冒号错写成点的情况
	if attr ~= nil and type(attr) == 'function' then
		return function(self, ...)
			if type(self) ~= 'table' and type(self) ~= 'userdata' then
				error(string.format('不是一个实例,有可能调用函数时你用了点,而不是冒号.总之参数错了'))
			end
			return attr(self, ...)
		end
	end
	return attr
end

local function __newIndexOfInstance(self, k, v)
	rawset(self, k, v) --用rawset避免死递归	
end

local function newInstance(cls, ...)--生成类的实例
	if not rawget(cls,'__isClass__') then
		error('cls竟然不是一个类',0)
	end
	local self
	local __new__ = cls.__new__ --获取cls.__new__有多态行为
	if __new__ then
		self = __new__(cls,...)
		if type(self) ~= 'table' and type(self) ~= 'userdata' then
			error(string.format('__new__函数必须返回一个table或userdata,你返回的是%s',type(self)),0)
		end
	else --已经不可能进这里了,因为全部类的父类object有__new__方法
		self = {}
	end
	setMetaTable(self,cls)
	self.__class__ = cls --标识这个实例所对应的类
	local __init__ = cls.__init__ --获取cls.__init__有多态行为
	if __init__ then
		__init__(self, ...)
	end
	return self
end

--是全部类的父类
cObject=setmetatable({}, {__call = newInstance})
cObject.__bases__ = {}
cObject.__isClass__ = true
cObject.__index = __indexOfInstance
cObject.__newindex = __newIndexOfInstance
cObject.__new__ = function (cls,...) return {} end

function class(...)--创建一个class
	local arg = {...}--lua 5.2 之后局部空间内不再自动生成arg,所以要手工生成一个
	for i, super in ipairs(arg) do
		if super[".isclass"]~=nil then
			error(string.format('要想从引擎继承%s,请自行实现__new__方法', tostring(super)),0)
		end
	end
	local cls = nil
	local iParentCount = table.getn(arg)
	if iParentCount == 0 then--是基类,没有父类
		cls = setmetatable({}, {__call=newInstance, __index=cObject})
	elseif iParentCount == 1 then--是子类,有一个父类
		cls = setmetatable({}, {__call=newInstance, __index=arg[1]})
	else--是子类,有多个父类
		cls = setmetatable({}, {__call=newInstance, __index=function(t, key) return searchAttr(key,arg) end})
	end
	cls.__bases__ = {...} --保存这个属性只是为了用于判断类与类,实例与类之间的关系,暂时没有用上

	cls.__newindex = __newIndexOfInstance
	cls.__index = __indexOfInstance -- cls.__index=cls
	cls.__isClass__ = true --方便检查一个表是不是类,获取时要用rawget,否则实例也会返回true
	return cls
end


--最基本的用法
--
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

--]]

--[[
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

--]]
--------------------------
----再次继承

--[[
local cPetDog=class(cDog)
	function cPetDog.voice(self)
		cDog.voice(self) --调用父类的同名方法
		print('cPetDog:miao miao')
	end
local oPetDog=cPetDog('wang cai',2,3)
oPetDog:voice()

print('---------------------------')


--]]

---以上都是单继承,接下来是多继承-------------------------
--[[
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

