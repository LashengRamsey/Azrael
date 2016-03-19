module("dbClient", package.seeall)

local giExecuteId = 0
local gtExecuteMap = {}

local function getExecuteId()
	giExecuteId = giExecuteId + 1
	if giExecuteId > 2147483646 then
		giExecuteId = 1
	end
	return giExecuteId
end

function select(table, args, callback, cbargs)
	
	local iCbId = getExecuteId

	gtExecuteMap[giExecuteId] = {callback, cbargs}
	local t = {
		iServerNo = G_ServerNo,	--服务器编号
		iCbId = giExecuteId,	--回调id

	}

	local send = {
		AccountStr = "robot1"
	}

	Net.sendToDB(Protocol.G2D_COMMAND, send)
end










