module("Protocol", package.seeall)

--保留1-100用于网关发消息到各个服务器
--gate to server
G2S_ClientConn = 1 		--收到客户端连接
G2S_ClientDisConn = 2 	--客户端连接断开
G2S_RevPackage = 3	--客户端网络包
S2G_SendPackage = 4	--发送客户端网络包


--上行:客户端-->>服务端
--client to gate
C2G_Login = 1000	--登录



--================================

--下行:服务端-->>客户端
--gate to server
G2C_Account_Info = 20000		--帐号信息







--下行:服务器-->>服务器
G2G_Test = 100000			--测试





--游戏服务器-->>DB服务器
G2D_COMMAND = 20000			--执行命令
D2G_COMMAND_RESULT = 20001	--结果







