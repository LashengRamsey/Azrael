module("dbServer", package.seeall)


function G2D_Common(session, packet)
	print("=======G2D_Common============")
	print_r(packet)
	local result = hiredis.hiredis_command("GET", "")
	--redis有数据
	if result ~= c_hiredis.NIL then
		--print("==========")

		return
	end

	--没有数据去数据库查询
	local sql = string.format([[select * from account where account='robot1']])
	result = mysqlClient.mysql_query(sql)
	print_r(result)
end







