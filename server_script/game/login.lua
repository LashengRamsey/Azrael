module("login", package.seeall)

function login(sessionObj, packet)
	print("======login==========")
	--print_r(packet)

	local send = {
		db_name = "account",
		id = packet.AccountStr,
	}
	
	local account = AccountMng:newAccount(sessionObj, packet.AccountStr)
	if account:Loading() then
		--正在登录中
		return
	elseif account:Loaded() then
		--已经完成登录
		return
	else
		account:setLoading(true)
	end

	dbClient.query(send, loginCallBack, account)
end

function loginCallBack(packet, account)
	print("======loginCallBack==========")
	--print_r(packet)
	--print(account)
	

	if #packet.tResult <= 0 then--没有数据

	else
		local info = packet.tResult[1]
		--print_r(info)
		account:updateInfo(info)

	end
end

--===================================================
--新创建帐号
function insert_account(account)
	local send = {
		
	}
end




