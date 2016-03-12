module("login", package.seeall)

function login(sessionObj)
	print("======login==========")
	local accountName = G_UnPacketS()
	print(accountName)
	local i = G_UnPacketI(1)
	print(i)


	dbClient.select(gdStructMap.account, accountName)
end



