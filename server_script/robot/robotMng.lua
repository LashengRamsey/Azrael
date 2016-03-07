module("robotMng", package.seeall)

--管理机器人

local gtRobotMap = {}	--机器人
local giRobotCnt = 1 	--机器人数量
local gbStartFlag = false --

function startRobot()
	timer.CallLater(robotInit, 2000)
end

function robotInit()
	if not gbStartFlag then
		gbStartFlag = true
		for i=1, giRobotCnt do
			local robotObj = robot.robot:new()
			table.insert(gtRobotMap, robotObj)
		end

		timer.CallLater(robotMngUpdate, 1000, nil, 1)
	end
end


function robotMngUpdate()
	--print("========robotMngUpdate========")
	for k, obj in ipairs(gtRobotMap) do
		--print(obj)
		obj:update()
	end
end




