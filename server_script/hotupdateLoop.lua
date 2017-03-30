
--https://github.com/asqbtcupid/lua_hotupdate


local HU = require "luahotupdate"
local test = require "test.testHotUpdate"


local HUFileName = "hotupdatelist"

function doHotUpdate()
	local FileNameList = doGetHUFileMap()
	if FileNameList then
		--print("========doHotUpdate====")
		HU.Update(FileNameList)
	end
	--test.func()
end

function doGetHUFileMap()
	file = io.open(HUFileName, "r")
	if not file then
		return nil
	end

	local FileNameList = {}
	io.input(file)
	for line in io.lines() do
		table.insert(FileNameList, line)
	end
	io.close(file)

	--删除文件
	if C_SystemName() == 1 then	--windows
		--io.popen("del " .. HUFileName)
	else
		--io.popen("rm " .. HUFileName)
	end
	return FileNameList
end


function startUpdateTimer()
	--代码路径
	scriptPath = {}
	file = nil
	if C_SystemName() == 1 then	--windows
		file = io.popen("cd")
	else
		file = io.popen("pwd")
	end
	io.input(file)
	for line in io.lines() do
		table.insert(scriptPath, line)
	end
	
	HU.Init(scriptPath)
	timer.CallLater(doHotUpdate, 3000, nil, 1)
end




