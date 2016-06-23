module("dbClient", package.seeall)

giCommandQuery = 1
giCommandUpdate = 2
giCommandInsert = 3

local giExecuteId = 0
local gtExecuteMap = {}

local function getExecuteId()
	giExecuteId = giExecuteId + 1
	if giExecuteId > 2147483646 then
		giExecuteId = 1
	end
	return giExecuteId
end

function command(type, dbName, args, callback, cbargs)
	local iCbId = 0
	if callback then
		iCbId = getExecuteId()
		gtExecuteMap[iCbId] = {cb = callback, args = cbargs}
	end
	local send = {
		iCbId = iCbId,	--回调id
		iType = type,
		db_name = dbName,
		sValue = tableToStr(args),
	}
	
	Net.sendToDB(Protocol.G2D_COMMAND, send)
end

function CommandCallBack(fn, packet)
	print("==========CommandCallBack==============")
	--print_r(packet)
	local iCbId = packet.iCbId
	if not iCbId then
		CLogError("dbClient.CommandCallBack error not iCbId")
		return
	end

	local tCb = gtExecuteMap[iCbId]
	gtExecuteMap[iCbId] = nil
	local func = tCb.cb
	if not tCb or not func then
		CLogError("dbClient.CommandCallBack error not tCb : iCbId = %d func = %s", iCbId, func)
		return
	end

	if packet.tResult then
		for k,v in ipairs(packet.tResult) do
			packet.tResult[k] = strToTable(v)
		end
	end
	--packet.tResult = strToTable(packet.tResult)
	--packet.sResult = nil
	func(packet, tCb.args)
end



--=========================================
--test dbclient
function testQuery()
	local send = {
		db_name = "test",
		id = "1",
	}
	

	dbClient.command(dbClient.giCommandQuery, send, testQueryCallBack, {1,2,3})
end

function testQueryCallBack(packet, args)
	--print("======testQueryCallBack=======")
	--print_r(args)
	--print_r(packet)
end




