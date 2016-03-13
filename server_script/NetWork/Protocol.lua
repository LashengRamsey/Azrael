module("Protocol", package.seeall)


--上行:客户端-->>服务端

C2G_Login = 1	--登录



--================================

--下行:服务端-->>客户端

G2C_Account_Info = 2000	--帐号信息







--下行:服务器-->>服务器
G2G_Test = 10000	--测试





--游戏服务器-->>DB服务器
G2D_COMMAND = 20000	--执行命令





