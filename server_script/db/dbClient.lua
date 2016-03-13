module("dbClient", package.seeall)

local giExecuteId = 0
local gtExecuteMap = {}

function select(table, key, callback, args)
	giExecuteId = giExecuteId + 1
	gtExecuteMap[giExecuteId] = {callback, args}
	local t = {


		iServerNo = G_ServerNo,
		iCbId = giExecuteId,

	}


	Net.sendToDB()
	print()
end










