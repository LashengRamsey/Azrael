module("login", package.seeall)

function login(sessionObj, packet)
	print("======login==========")
	print_r(packet)

	local send = {
		db_name = "account",
		key = "account",
		id = "robot1"--packet.AccountStr,
	}
	

	dbClient.select(send, loginCallBack, nil)
end

function loginCallBack(result)

end


