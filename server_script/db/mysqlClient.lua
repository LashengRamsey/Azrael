module("mysqlClient", package.seeall)

local mysql_ip = nil
local mysql_port = nil
local mysql_db_name = nil
local mysql_user = nil
local mysql_passwd = nil

local mysql_context = nil

function mysql_init()
	print("========mysql_init===========")

	mysql_ip = C_GetConfig("mysql_ip")
	mysql_port = C_GetConfig("mysql_port")
	mysql_db_name = C_GetConfig("mysql_name")
	mysql_user = C_GetConfig("mysql_user")
	mysql_passwd = C_GetConfig("mysql_passwd")
	--print(c_mysql_connect)
	CLogInfo("LogInfo", "mysql_ip = " .. mysql_ip)
	CLogInfo("LogInfo", "mysql_port = " .. mysql_port)
	CLogInfo("LogInfo", "mysql_db_name = " .. mysql_db_name)
	CLogInfo("LogInfo", "mysql_user = " .. mysql_user)
	CLogInfo("LogInfo", "mysql_passwd = " .. mysql_passwd)

	mysql_connect()
end

local function testSql()
	-- mysql_insert([[insert into test (tid, name) values (1, "1_name")]])
	-- mysql_query([[select tid, name from test where tid = 1]])
	-- mysql_update([[update test set name = "2_name" where tid=1]])
end

--连接mysql数据库
local function mysql_connect()
	CLogInfo("LogInfo", "mysql_connect start：%s,%s,%s,%s,%s", mysql_ip, mysql_user, mysql_passwd, mysql_db_name, mysql_port)
	--有返回说明连接数据库成功
	mysql_context = c_mysql_connect(mysql_ip, mysql_user, mysql_passwd, mysql_db_name, mysql_port)
	CLogInfo("LogInfo", "mysql_connect success")
	--testSql()
end

--断开数据连接
function mysql_disconntect()
	c_mysql_disconntect(mysql_context)
	mysql_context = nil
	CLogError("error", "mysql disconntect")
end

--执行query语句
--//return double dimesional lua table
function mysql_query(sql)
	if not mysql_context then
		CLogError("error", "msyql mysql_query error:mysql_context is nil")
		return
	end
	CLogInfo("LogInfo", "mysql mysql_query sql:%s", sql)
	local result,tFields  = c_mysql_query(mysql_context, sql)
	if result == -1 then
		CLogError("error", "msyql mysql_query error: result = -1, parameter num error")
	elseif result == -2 then
		CLogError("error", "msyql mysql_query error: result = -2,mysql_ping error")
	elseif result == -3 then
		CLogError("error", "msyql mysql_query error: result = -3,mysql_query error sql = " .. sql)
	elseif result == 0 then
		CLogInfo("LogInfo", "mysql mysql_query success sql = " .. sql)
		--print_r(tFields)
	end
	
	return tFields
end


--执行插入语句
--return inset_id
function mysql_insert(sql)
	if not mysql_context then
		CLogError("error", "msyql insert error:mysql_context is nil")
		return -1
	end
	local result,inset_id = c_mysql_insert(mysql_context, sql)

	CLogInfo("LogInfo", "mysql mysql_insert")
	if result == -1 then
		CLogError("error", "msyql insert error: result = -1, parameter num error")
	elseif result == -2 then
		CLogError("error", "msyql insert error: result = -2,mysql_ping error")
	elseif result == -3 then
		CLogError("error", "msyql insert error: result = -3,insert error sql = " .. sql)
	elseif result == 0 then
		CLogInfo("LogInfo", "mysql mysql_insert success sql = " .. sql)
	end
end

--执行update
function mysql_update(sql)
	if not mysql_context then
		CLogError("error", "msyql update error:mysql_context is nil")
		return
	end
	local result = c_mysql_update(mysql_context, sql)

	CLogInfo("LogInfo", "mysql mysql_update")
	if result == -1 then
		CLogError("error", "msyql update error: result = -1, parameter num error")
	elseif result == -2 then
		CLogError("error", "msyql update error: result = -2,mysql_ping error")
	elseif result == -3 then
		CLogError("error", "msyql update error: result = -3,update error sql = " .. sql)
	elseif result == 0 then
		CLogInfo("LogInfo", "mysql mysql_update success sql = " .. sql)
	end
end


function mysql_character_set( sql )
	if not mysql_context then
		CLogError("error", "msyql character_set error:mysql_context is nil")
		return
	end
	local result = c_mysql_character_set(mysql_context, sql)

	CLogInfo("LogInfo", "mysql c_mysql_character_set")
	if result == -1 then
		CLogError("error", "msyql c_mysql_character_set error: result = -1, parameter num error")
	elseif result == 0 then
		CLogInfo("LogInfo", "mysql mysql_character_set success sql = " .. sql)
	end
end

