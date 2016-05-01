
local pst = class()

function pst:__init__()
	--print("==========pst:__init__========")
	self.__tData = {}
	self.eDirtyEvent = cEvent()
end

--标示为脏数据
function pst:markDirty()
	self:_onDirty()
end

--触发事件
function pst:_onDirty()
	--self.eDirtyEvent()
end

function pst:save()
	return self.__tData--.copy()
	--因为返回dict后子类会往里面加东西,dict是引用类型,导致永久性存在
	--返回一个浅拷贝,即使被修改也不会影响到原来的dict
end

function pst:load(tData)
	self.__tData = tData
end	

--返回成功后的结果值
function pst:add(sKey, iValue, iDefault)
	iDefault = iDefault or 0
	self.__tData[sKey] = table.get(self.__tData, sKey, iDefault) + iValue
	self:markDirty()
	return self.__tData[sKey]
end

function pst:delete(sKey, uDefault)
	local tmp = self.__tData[sKey] or uDefault
	if tmp then
		self.__tData[sKey] = nil
		self:markDirty()
    end
	return tmp
end

function pst:set(sKey, uValue)
	--只能保存数据或字符串？
	sType = type(uValue)
	if sType ~= "string" and sType ~= "number" and sType ~= "table" then
		print("error pst:set type(uValue)= " .. sType)
		return
	end

	self.__tData[sKey] = uValue
	self:markDirty()
	return self --可以链式调用
end

--默认值是0更合理
function pst:fetch(sKey, uDefault)
	return self.__tData[sKey] or uDefault-- or 0
end

function pst:hasKey(uKey)
	return (self.__tData[sKey] and true) or true
end

--Flag	需要处理的标志位
--bVal	标志位取正还是取反
function pst:setFlag(iFlag, bVal)
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
function pst:getFlag(iFlag)
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

function test_pst()
	local obj = pst()
    print("======test_pst=========")
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
--test_pst()
