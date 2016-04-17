module("dbServer", package.seeall)



function G2DExeCommand(iServerNo, packet)
	print("=======G2D_Common============")
	--print_r(packet)
	if packet.iType == dbClient.giCommandQuery then
		query(iServerNo, packet)
	elseif packet.iType == dbClient.giCommandUpdate then
		update(iServerNo, packet)
	elseif packet.iType == dbClient.giCommandInsert then
		insert(iServerNo, packet)
	end

end

function query(iServerNo, packet)
	print("=======query============")
	--print_r(iServerNo)
	local tDbTable = dbStruct.gDbTableKey[packet.db_name]
	local tValue = strToTable(packet.sValue)
	local uTableKey = tValue[tDbTable[1]]
	local redisKey = "" .. packet.db_name .. uTableKey	--表名字+id
	local sResult = hiredis.hiredis_command("GET", redisKey)
	--print(redisKey)
	--print(sResult)
	--redis有数据
	if sResult == c_hiredis.NIL then
		--没有数据去数据库查询
		local sql = ""
		if tDbTable[2] ==  dbStruct.giKeyInt then
			sql = string.format([[select * from %s where %s=%s]], packet.db_name, tDbTable[1], uTableKey)
		else
			sql = string.format([[select * from %s where %s='%s']], packet.db_name, tDbTable[1], uTableKey)
		end
		--print(sql)
		local tField = mysqlClient.mysql_query(sql)
		sResult = tableToStr(tField)
		hiredis.hiredis_command("SET", redisKey, sResult)
	end

	--print(sResult)
	local send = {
		iCbId = packet.iCbId,
		tResult = {}
	}
	if sResult == c_hiredis.NIL or sResult == "" then
		send.result = -1
	else
		send.result = 0
		table.insert(send.tResult, sResult)
	end
	--print_r(send)
	Net.sendToGame(iServerNo, Protocol.D2G_COMMAND_RESULT, send)
end


function update(iServerNo, packet)
	print("=======update============")
	--print(iServerNo)
	local tDbTable = dbStruct.gDbTableKey[packet.db_name]
	local tValue = strToTable(packet.sValue)
	local uTableKey = tValue[tDbTable[1]]
	
	local redisKey = "" .. packet.db_name .. uTableKey	--表名字+id
	if hiredis.hiredis_command("SET", redisKey, packet.sValue) == c_hiredis.NIL then
		--redis没有保存
	end
	

	local tDbTableInfo = dbStruct.gDbTableInfo[packet.db_name]
	print_r(tDbTableInfo)
	--update account set where 'Account' = 
	local sql = string.format("update %s set ", packet.db_name)
	for k, v in pairs(tDbTableInfo) do
		if v == dbStruct.giKeyInt then
			sql = string.format("%s %s=%d,", sql, k, tValue[k])
		else
			sql = string.format([[%s %s='%s',]], sql, k, tValue[k])
		end
	end
	sql = string.sub(sql, 1, string.len(sql)-1)
	if tDbTable[2] ==  dbStruct.giKeyInt then
		sql = string.format([[%s where %s=%s]], sql, tDbTable[1], uTableKey)
	else
		sql = string.format([[%s where %s='%s']], sql, tDbTable[1], uTableKey)
	end
	--print(sql)
	local tField = mysqlClient.mysql_update(sql)


	if packet.iCbId == 0 then
		return
	end
	local send = {
		iCbId = packet.iCbId,
		tResult = {}
	}
	Net.sendToGame(iServerNo, Protocol.D2G_COMMAND_RESULT, send)
end

function insert(iServerNo, packet)
	print("=======insert============")
	local tDbTable = dbStruct.gDbTableKey[packet.db_name]
	local tValue = strToTable(packet.sValue)
	local uTableKey = tValue[tDbTable[1]]
	--插入mysql
	--local sql = [[INSERT account SET Account="robot2",Lv=1,EXP=0,gold=0,VipLv=0,LastLoginTime=0,CreateTime=0,DATA=""]]
	local sql = string.format("INSERT %s SET ", packet.db_name)
	local tDbTable = dbStruct.gDbTableInfo[packet.db_name]
	for k, v in pairs(tDbTable) do
		if v == dbStruct.giKeyInt then
			sql = string.format("%s %s=%d,", sql, k, tValue[k])
		else
			sql = string.format([[%s %s='%s',]], sql, k, tValue[k])
		end
	end

	sql = string.sub(sql, 1, string.len(sql)-1)
	--print(sql)
	CLogInfo("LogInfo", sql)
	local result = mysqlClient.mysql_insert(sql)

	--更新redis
	if result == 0 then
		local redisKey = "" .. tValue.db_name .. uTableKey	--表名字+id
		hiredis.hiredis_command("SET", redisKey, packet.sValue)
	end

	if packet.iCbId == 0 then
		return
	end
	local send = {
		iCbId = packet.iCbId,
		tResult = {}
	}
	Net.sendToGame(iServerNo, Protocol.D2G_COMMAND_RESULT, send)
end


