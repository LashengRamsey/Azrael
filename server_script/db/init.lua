--print(package.path)
--package.path = "../server_script/?.lua;" .. package.path
--package.path = "./?.lua" .. package.path
--print(package.path)
--C++层类
C_LuaNetWork = LuaNetwork		--网络
C_Bit = Bit						--位操作

C_Connection = Connection 		--连接



--启动服务器C层入口
function c_main()
	print("=====dbb=c_main=====1111==========")
	--require "test"
	require "db.loadRequire"
	require "NetWork.PacketHandler"

	init()
end

--定时器，C层调用
function CHandlerTimer(id)
	print("====db =CHandlerTimer============")
	return timer.DoTimer(id)
end


G_ServerNo = 0

function init()
	print("====db==init===============")
	G_ServerNo = C_GetServerID()
	print("G_ServerNo = " .. G_ServerNo)

	PacketHandler.initDbPacketHandler()

	mysqlClient.mysql_init()
	hiredis.hiredis_init()
	--timer.CallLater(TestSendPacket, 1000)
end

function CHandlerMsg(target, sn, eid, fid, data, startPos, size)
	Net.doHandlerMsg(target, sn, eid, fid, data, startPos, size)
end

function CHandlerConnect(sn)
	print("========CHandlerConnect=============")
	print("sn = " .. sn)
	Session.newSession(sn)
end

function CHandlerDisconnect(sn)
	print("=======CHandlerDisconnect==============")
	Session.delSession(sn)	
end

--错误信息
function CHandlerError(err)
	print("========CHandlerError=============")
	--print(err)
end

function CHandlerNetMsg(sn, data, startPos, size)
	print("========CHandlerNetMsg=============")
	Net.doHandlerMsg(0, sn, 0, 0, data, startPos, size)
end

