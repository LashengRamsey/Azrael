local ClientSession = require "gate.ClientSession"

G_ServerNo = 0

--启动服务器C层入口
function c_main()
	G_ServerNo = C_GetServerID()
	print("gate G_ServerNo = " .. G_ServerNo)
	require "gate.loadRequire"
	require "NetWork.PacketHandler"
	init()
end

--定时器，C层调用
function CHandlerTimer(id)
	return timer.DoTimer(id)
end

function init()
	G_InitLog("gate")
	PacketHandler.initGamePacketHandler()
	timer.CallLater(testDb, 1000)
	startUpdateTimer()--热更新定时器
end

--src:发来服务器编号
--fid：消息类型
--sn:连接id
--data：数据
--startPos：数据开始下标
--size：数据大小
function CHandlerMsg(src, sn, fid, data, startPos, size)
	print("src = " .. src)
	print("sn = " .. sn)
	Net.doHandlerMsg(src, sn, fid, data, startPos, size)
end

function CHandlerConnect(sn)
	print("========CHandlerConnect=============")
	print("sn = " .. sn)
	ClientSession.newSession(sn)
end

function CHandlerDisconnect(sn)
	print("=======CHandlerDisconnect==============")
	ClientSession.delSession(sn)	
end

--错误信息
function CHandlerError(err)
	print("========CHandlerError=============")
	CLogError("error", err)
end

function CHandlerNetMsg(sn, data, startPos, size)
	print("========CHandlerNetMsg=============")
	Net.doHandlerMsg(0, sn, 0, data, startPos, size)
end