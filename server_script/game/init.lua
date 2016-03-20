

G_ServerNo = 0

--启动服务器C层入口
function c_main()
	print("======c_main=====1111==========")
	G_ServerNo = C_GetServerID()
	print("G_ServerNo = " .. G_ServerNo)
	require "game.loadRequire"
	require "NetWork.PacketHandler"
	init()
	
end

--定时器，C层调用
function CHandlerTimer(id)
	print("=====CHandlerTimer============")
	return timer.DoTimer(id)
end

function init()
	print("======init===============")
	PacketHandler.initGamePacketHandler()
	timer.CallLater(TestSendPacket, 1000)
end

--target:发来服务器编号
--fid：消息类型
--sn:连接id
--data：数据
--startPos：数据开始下标
--size：数据大小
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
