module("hiredis", package.seeall)

local gsRedisIp = nil
local giRedisPort = nil

local guRedisConn = nil

function hiredis_init()
	print("========hiredis_init===========")
	gsRedisIp = C_GetConfig("redis_ip")
	giRedisPort = tonumber(C_GetConfig("redis_port"))

	CLogInfo("gsRedisIp = " .. gsRedisIp)
	CLogInfo("giRedisPort = " .. giRedisPort)
	hiredis_connect()
end

--连接redis服务器
function hiredis_connect()
	if not gsRedisIp and not giRedisPort then
		C_Error("hiredis_connect error: gsRedisIp or giRedisPort is nil")
		return
	end
	
	local conn = c_hiredis.connect(gsRedisIp, giRedisPort, 0)
	--print(type(conn))
	if not conn then
		C_Error("hiredis_connect failed")
		timer.CallLater(hiredis_connect, 10000)
	elseif type(conn) == "string" then
		C_Error("hiredis_connect failed:%s", conn)
		timer.CallLater(hiredis_connect, 10000)
	elseif type(conn) == "userdata" then
		--print_r(conn)
		CLogInfo("hiredis_connect success")
		guRedisConn = conn
		--hiredis_close()
		print(hiredis_command("PING"))
		--print(hiredis_command("SET", "fuck", "you"))
		--print(hiredis_command("GET", "fuck"))
	end
end

--关闭redis服务器
function hiredis_close()
	if not guRedisConn then 
		C_Error("hiredis_close error:redis server not connect")
		return
	end
	guRedisConn:close()
	CLogInfo("hiredis_close success")
end

--执行redis命令
function hiredis_command(...)
	if not guRedisConn then 
		C_Error("hiredis_command error:redis server not connect")
		return
	end
	local nret = guRedisConn:command(...)
	CLogInfo("hiredis_command success nret = ")
	--print(nret)
	--print(type(nret))
	return nret
end



