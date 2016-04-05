module("dbServer", package.seeall)



function G2DExeCommand(iServerNo, packet)
	print("=======G2D_Common============")
	print_r(packet)
	if packet.iType == dbClient.giCommandQuery then
		query(iServerNo, packet)
	elseif packet.iType == dbClient.giCommandUpdate then

	elseif packet.iType == dbClient.giCommandInsert then
		insert(iServerNo, packet)
	end

end

function query(iServerNo, packet)
	print("=======G2D_Common============")
	--print_r(iServerNo)
	local tValue = strToTable(packet.sValue)
	local redisKey = "" .. tValue.db_name .. tValue.id	--表名字+id
	local sResult = hiredis.hiredis_command("GET", redisKey)
	--redis有数据
	if sResult == c_hiredis.NIL then
		--没有数据去数据库查询
		local tDbTable = dbStruct.gDbTableKey[tValue.db_name]
		local sql = ""
		if tDbTable[2] ==  dbStruct.giKeyInt then
			sql = string.format([[select * from %s where %s=%s]], tValue.db_name, tDbTable[1], tValue.id)
		else
			sql = string.format([[select * from %s where %s='%s']], tValue.db_name, tDbTable[1], tValue.id)
		end
		 
		local tField = mysqlClient.mysql_query(sql)
		sResult = tableToStr(tField)
		hiredis.hiredis_command("SET", redisKey, sResult)
	end

	--print(sResult)
	local send = {
		iCbId = packet.iCbId
	}
	if sResult == c_hiredis.NIL or sResult == "" then
		send.result = -1
		send.sResult = ""
	else
		send.result = 0
		send.sResult = sResult
	end
	--print_r(send)
	Net.sendToGame(iServerNo, Protocol.D2G_COMMAND_RESULT, send)
end


function G2DUpdate(iServerNo, packet)
	local redisKey = "" .. packet.db_name .. packet.id	--表名字+id
	hiredis.hiredis_command("SET", redisKey, sResult)
	
end

function insert(iServerNo, packet)
	local tValue = strToTable(packet.sValue)
	--插入mysql
	--local sql = [[INSERT account SET Account="robot2",Lv=1,EXP=0,gold=0,VipLv=0,LastLoginTime=0,CreateTime=0,DATA=""]]
	local sql = "INSERT " .. tValue.db_name .. " SET "
	local tDbTable = dbStruct.gDbTableInfo[tValue.db_name]
	for k, v in pairs(tDbTable) do
		local s = ""
		if v == dbStruct.giKeyInt then
			s = string.format("%s=%d,", k, tValue[k])
		else
			s = string.format([[%s='%s',]], k, tValue[k])
		end
		sql = sql .. s
	end

	sql = string.sub(sql, 1, string.len(sql)-1)
	print(sql)
	CLogInfo("LogInfo", sql)
	local result = mysqlClient.mysql_insert(sql)

	--更新redis
	if result == 0 then
		local redisKey = "" .. tValue.db_name .. tValue.id	--表名字+id
		hiredis.hiredis_command("SET", redisKey, packet.sValue)
	end

	local send = {
		iCbId = packet.iCbId,
		result = result,
	}
	Net.sendToGame(iServerNo, Protocol.D2G_COMMAND_RESULT, send)
end


