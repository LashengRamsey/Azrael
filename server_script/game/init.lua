
G_ServerId = 0

--启动服务器C层入口
function c_main()
	print("======c_main=====1111==========")
	G_ServerId = C_GetServerID()
	print("G_ServerId = " .. G_ServerId)
	require "game.loadRequire"
	require "game.PacketHandler"
	init()
	
end

--定时器，C层调用
function CHandlerTimer(id)
	print("=====CHandlerTimer============")
	return timer.DoTimer(id)
end

function init()
	PacketHandler.initPacketHandler()
	print("======init===============")
	--CLogInfo("CLogInfo:CLogInfo CLogInfo CLogInfo = %d sadfsa %d sdfs %s sdf %d", 1, 1 , "1231",4)
	--timer.CallLater(Net.TestSendPacket, 1000)
	--Connect.Connect_test()
end

function CHandlerMsg(target, sn, eid, fid, data, startPos, size)
	Net.doHandlerMsg(target, sn, eid, fid, data, startPos, size)
end

function CHandlerConnect(...)
	print("========CHandlerConnect=============")
	for i=1, select("#", ...) do
		print("CHandlerConnectCHandlerConnect")
	end
end

function CHandlerDisticonnect()
	print("=======CHandlerDisticonnect==============")	
end

--错误信息
function CHandlerError(err)
	print("========CHandlerError=============")
	--print(err)
end

