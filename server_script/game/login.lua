module("login", package.seeall)

function login(sessionObj, packet)
	print("======login==========")
	--print_r(packet)

	local send = {
		db_name = "account",
		id = "robot1"--packet.AccountStr,
	}
	

	dbClient.query(send, loginCallBack)
end

function loginCallBack(packet)
	print("======loginCallBack==========")
	--print_r(packet)
end






