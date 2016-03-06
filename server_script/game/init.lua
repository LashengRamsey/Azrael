--print(package.path)
--package.path = "../server_script/?.lua;" .. package.path
--package.path = "./?.lua" .. package.path
--print(package.path)
--C++层类
C_LuaNetWork = LuaNetwork		--网络
C_Bit = Bit						--位操作

C_Connection = Connection 		--连接
--C++层类函数


--启动服务器C层入口
function c_main()
	print("\n\n\n\n\n\n")
	print("======c_main=====1111==========")

	--require "test"

	require "game.loadRequire" 
	

	print(C_Bit)
	print(C_Bit.bnot)
	print(C_LuaNetWork)
	

	init()
end

--定时器，C层调用
function CHandlerTimer(id)
	print("=====CHandlerTimer============")
	return timer.DoTimer(id)
end


G_ServerId = 0

function init()
	print("======init===============")
	G_ServerId = C_GetServerID()
	print("G_ServerId = " .. G_ServerId)
	CLogInfo("CLogInfo:CLogInfo CLogInfo CLogInfo = %d sadfsa %d sdfs %s sdf %d", 1, 1 , "1231",4)
	--timer.CallLater(Net.TestSendPacket, 1000)
	Connect.Connect_test()
end


function CHandlerMsg(target, sn, eid, fid, data, startPos, size)
	Net.doHandlerMsg(target, sn, eid, fid, data, startPos, size)
end


function CHandlerConnect()
	print("========CHandlerConnect=============")
end

function CHandlerDisticonnect()
	print("=======CHandlerDisticonnect==============")	
end

--错误信息
function CHandlerError(err)
	print("========CHandlerError=============")
	--print(err)
end















