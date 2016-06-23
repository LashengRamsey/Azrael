module("SvrType", package.seeall)

G_SERVER_TYPE = nil

G_SERVER_GAME = 1	--游戏服
G_SERVER_DB = 2		--DB服
G_SERVER_FRIEND = 3 --好友服
G_SERVER_COMM = 4	--公共服
G_SERVER_LOGIN = 5	--登录服
G_SERVER_GATE = 6	--网关服
G_SERVER_CHAT = 7	--聊天服
G_SERVER_SCENE = 8	--场景服
G_SERVER_FIGHT = 9	--战斗服

G_SERVER_ROBOT = 1000	--机器人

function setSvrType(iType)
	G_SERVER_TYPE = iType
end

function getSvrType()
	return G_SERVER_TYPE
end

--是否为主服
function IsGameServer()
	return G_SERVER_TYPE == G_SERVER_GAME
end

--是否DB服
function IsDBServer()
	return G_SERVER_TYPE == G_SERVER_DB
end

--是否为好友服
function IsFriendServer()
	return G_SERVER_TYPE == G_SERVER_FRIEND
end

--是否为公共服
function IsCommServer()
	return G_SERVER_TYPE == G_SERVER_COMM
end

--是否为网关服
function IsGateServer()
	return G_SERVER_TYPE == G_SERVER_GATE
end

--是否为登录服
function IsLoginServer()
	return G_SERVER_TYPE == G_SERVER_LOGIN
end

--是否为聊天服
function IsChatServer()
	return G_SERVER_TYPE == G_SERVER_CHAT
end

--是否为场景服
function IsSceneServer()
	return G_SERVER_TYPE == G_SERVER_SCENE
end

--是否为战斗服
function IsFightServer()
	return G_SERVER_TYPE == G_SERVER_FIGHT
end
