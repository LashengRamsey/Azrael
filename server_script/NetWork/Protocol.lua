module("Protocol", package.seeall)

--============================================
-- 约定：为了网关区分收到的客户端网络包，应该发送到哪个服务器进行处理，
-- 		用协议范围来进行区分，进行以下约定
-- 1、网关跟服务器之间使用的保留范围：[1, 9999]
-- 2、主游戏服处理的协议范围：[10000,19999]
-- 3、好友服处理的协议范围：[20000,29999]
-- 4、聊天服处理的协议范围：[30000,39999]
-- 5、场景服处理的协议范围：[40000,49999]
-- 6、战斗服处理的协议范围：[50000,59999]
-- ......以此类推
-- n、服务器之前的协议范围：[100000, 299999],这个协议不需要经过网关
--============================================

require "SvrType"

--根据协议号返回应该发送到哪个服
function whereToSend(protocol)
	if 10000 <= protocol and protocol <= 19999 then	--游戏服
		return SvrType.G_SERVER_GAME
	elseif 20000 <= protocol and protocol <= 29999 then	--好友服
		return SvrType.G_SERVER_FRIEND
	elseif 30000 <= protocol and protocol <= 39999 then	--聊天服
		return SvrType.G_SERVER_CHAT
	elseif 40000 <= protocol and protocol <= 49999 then	--场景服
		return SvrType.G_SERVER_SCENE
	elseif 50000 <= protocol and protocol <= 59999 then	--战斗服
		return SvrType.G_SERVER_FIGHT
	end
	--找不到往游戏服发，有可能被恶意利用
	return SvrType.G_SERVER_GAME
end


--保留1-10000用于网关发消息到各个服务器
--gate to server
G2S_ClientConn = 1 		--收到客户端连接
G2S_ClientDisConn = 2 	--客户端连接断开
G2S_RevPackage = 3		--客户端网络包
S2G_SendPackage = 4		--发送客户端网络包


--上行:客户端-->>服务端
--client to gate
C2G_Login = 10000	--登录



--================================

--下行:服务端-->>客户端
--gate to server
G2C_Account_Info = 20000		--帐号信息







--下行:服务器-->>服务器
G2G_Test = 100000				--测试





--游戏服务器-->>DB服务器
G2D_COMMAND = 200000			--执行命令
D2G_COMMAND_RESULT = 200001		--结果







