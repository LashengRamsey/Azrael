module("dbServer", package.seeall)


function G2D_Common(session, packet)
	print("=======G2D_Common============")
	print_r(session)
	local redisKey = "" .. packet.db_name .. packet.id	--表名字+id
	local result = hiredis.hiredis_command("GET", redisKey)
	--redis有数据
	if result ~= c_hiredis.NIL then
		--print("==========")

		return
	end

	--没有数据去数据库查询
	local sql = string.format([[select * from %s where %s='%s']], packet.db_name, packet.key, packet.id)
	local tField = mysqlClient.mysql_query(sql)
	print_r(tField)
	local send = {}
	if #tField <= 0 then
		send.result = 0
		send.args = ""
	else
		send.result = 0
		send.args = tableToStr(tField)
	end
	print_r(send)

end







