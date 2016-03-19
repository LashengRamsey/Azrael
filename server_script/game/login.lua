module("login", package.seeall)

function login(sessionObj, packet)
	print("======login==========")
	print_r(packet)

	

	dbClient.select(dbStruct.gdStructMap.account, packet, loginCallBack, nil)
end

function loginCallBack(result)

end


