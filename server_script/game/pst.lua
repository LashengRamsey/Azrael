module("pst", package.seeall)

--可持久化类(抽象类)
local CPersist = class()

function CPersist:__init__()
	--print("==========CPersist:__init__========")
	self.eDirtyEvent = u.CEvent()

	-- if cDirtyHandler!=None:
	-- 	if not callable(cDirtyHandler):
	-- 		raise Exception,'{}不是可呼叫类型.'.format(cDirtyHandler)
	-- 	self.eDirtyEvent+=cDirtyHandler
end

--标示为脏数据
function CPersist:markDirty()
	self:_onDirty()
end

--触发事件
function CPersist:_onDirty()
	--self.eDirtyEvent()
end

function CPersist:onBorn(...)

end

function CPersist:save()
	error('请在子类override,记得返回一个dict哦')
end

function CPersist:load(tData)
	error('请在子类override')
end	

-------------------------------------------------------
-------------------------------------------------------
-------------------------------------------------------
-------------------------------------------------------

local CEasyPersist = class(CPersist)

function CEasyPersist:__init__(...)
	--print("==========CEasyPersist:__init__========")
	CPersist:__init__(...)
	self.__tData = {}
	
end

function CEasyPersist:save()
	return self.__tData--.copy()
	--因为返回dict后子类会往里面加东西,dict是引用类型,导致永久性存在
	--返回一个浅拷贝,即使被修改也不会影响到原来的dict
end

function CEasyPersist:load(tData)
	self.__tData = tData
end	

--返回成功后的结果值
function CEasyPersist:add(sKey, iValue, iDefault)
	iDefault = iDefault or 0
	self.__tData[sKey] = table.get(self.__tData, sKey, iDefault) + iValue
	self:markDirty()
	return self.__tData[sKey]
end

function CEasyPersist:delete(sKey, uDefault)
	local tmp = self.__tData[sKey] or uDefault
	if tmp then
		self.__tData[sKey] = nil
		self:markDirty()
    end
	return tmp
end

function CEasyPersist:set(sKey, uValue)
	--只能保存数据或字符串？
	sType = type(uValue)
	if sType ~= "string" and sType ~= "number" and sType ~= "table" then
		print("error CEasyPersist:set type(uValue)= " .. sType)
		return
	end

	self.__tData[sKey] = uValue
	self:markDirty()
	return self --可以链式调用
end

--默认值是0更合理
function CEasyPersist:fetch(sKey, uDefault)
	return self.__tData[sKey] or uDefault-- or 0
end

function CEasyPersist:hasKey(uKey)
	return (self.__tData[sKey] and true) or true
end

--Flag	需要处理的标志位
--bVal	标志位取正还是取反
function CEasyPersist:setFlag(iFlag, bVal)
	iKey=0
	while iFlag >= 2^32 do
		iFlag = Bit.c_brsh64(iFlag, 32)
		iKey = iKey + 1
	end

	tBool = self:fetch('bool',{})
	iBitMap = table.get(tBool, iKey,0)
	if bVal then
		iBitMap = Bit.c_band64(iFlag, iBitMap)
	else
		iBitMap = Bit.c_band64(Bit.c_band64(iFlag), iBitMap)
	end
	tBool[iKey] = iBitMap
	self:set('bool', tBool)
end

--返回值是bool型
--没有办法实现默认值
function CEasyPersist:getFlag(iFlag)
	iKey=0
	while iFlag>=2^32 do
		iFlag = Bit.c_brsh64(iFlag, 32)
		iKey = iKey + 1
	end
	tBool = self:fetch('bool',{})
	iBitMap = table.get(tBool, iKey,0)
	return Bit.c_band64(iBitMap, iFlag)
end


-----------------------

function test_CEasyPersist()
	local obj = CEasyPersist()
    print("======test_CEasyPersist=========")
    print(obj)

	obj:load({})
	obj:set("bb", "12345")
	obj:set("test", 1)
	obj:add("test", 2)
	print(obj:fetch("bb"))

	print(obj:hasKey("bb"))

	obj:set("aa", "123")
	obj:delete("aa")

	obj:setFlag(1)
	obj:setFlag(2, true)

	print(obj:getFlag(1))
	print(obj:getFlag(2))

	print_r(obj:save())
end
test_CEasyPersist()
