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


G_ServerId = 0

function init()
	print("=====robot=init===============")
	G_ServerId = C_GetServerID()
	robotMng.startRobot()
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















