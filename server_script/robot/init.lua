--C++层类函数


--启动服务器C层入口
function c_main()
	print("======c_main robot===============")
	require "robot.loadRequire" 
	init()
end

--定时器，C层调用
function CHandlerTimer(id)
	--print("=====CHandlerTimer============")
	return timer.DoTimer(id)
end


G_ServerNo = 0

function init()
	print("=====robot=init===============")
	G_ServerNo = C_GetServerID()
	G_InitLog("robot")
	robotMng.startRobot()
end


function CHandlerMsg(src, sn, fid, data, startPos, size)
	Net.doHandlerMsg(src, sn, fid, data, startPos, size)
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
	Net.doHandlerMsg(0, sn, 0, data, startPos, size)
end


