module("SvrType", package.seeall)

G_SERVER_TYPE = nil

G_SERVER_GAME = 1	--游戏服
G_SERVER_DB = 2		--DB服
G_SERVER_FRIEND = 3 --好友服
G_SERVER_COMM = 4	--公共服
G_SERVER_LOGIN = 5	--登录服

G_SERVER_ROBOT = 6	--机器人

function setSvrType(iType)
	G_SERVER_TYPE = iType
end

function getSvrType()
	return G_SERVER_TYPE
end

