module("dbServer", package.seeall)


function G2D_Common(iServerNo, packet)
	print("=======G2D_Common============")
	--print_r(iServerNo)
	local redisKey = "" .. packet.db_name .. packet.id	--表名字+id
	local sResult = hiredis.hiredis_command("GET", redisKey)
	--redis有数据
	if sResult == c_hiredis.NIL then
		--没有数据去数据库查询
		local tDbTable = dbStruct.gdStructMap[packet.db_name]
		local sql = ""
		if tDbTable[2] ==  dbStruct.giKeyInt then
			sql = string.format([[select * from %s where %s=%s]], packet.db_name, tDbTable[1], packet.id)
		else
			sql = string.format([[select * from %s where %s='%s']], packet.db_name, tDbTable[1], packet.id)
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







