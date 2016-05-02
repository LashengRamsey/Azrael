
--c层封装的函数

--网络
C_LuaNetWork = LuaNetwork
C_ConnectRouter = C_LuaNetWork.connectRouter
C_ConnectDB = C_LuaNetWork.connectDB
C_GetSessionIP = C_LuaNetWork.getSessionIP
C_SetSessionUserData = C_LuaNetWork.setSessionUserData
C_closeSession = C_LuaNetWork.CloseSession
C_SendToServer = C_LuaNetWork.sendToServer
C_SendToNet = C_LuaNetWork.sendToNet
C_SendToDB = C_LuaNetWork.sendToDB
C_SendToGameServer = C_LuaNetWork.sendToGameServer


--连接
C_Connection = Connection
-- c_Connect
-- c_Write
-- c_Close
-- c_IsConnect
-- c_RawWrirte

--日志
-- C_TableToStr
-- C_Log
-- C_Info
-- C_Error
-- C_GetServerID
-- C_GetConfig
-- C_GetMTime
-- C_StopServer
-- C_GetHashCode
-- C_ToNumber
-- C_SystemName
