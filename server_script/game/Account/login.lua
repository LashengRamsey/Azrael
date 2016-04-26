module("login", package.seeall)

function login(sessionObj, packet)
	print("======login==========")
	--print_r(packet)

	local send = {
		Account = packet.AccountStr,
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

	dbClient.command(dbClient.giCommandQuery, "account", send, loginCallBack, account)
end

function loginCallBack(packet, account)
	print("======loginCallBack==========")
	--print_r(packet)
	--print(account)
	if not account then
		return
	end

	if packet.result == 0 then
		local info = packet.tResult[1]
		--print_r(info)
		account:updateInfo(info)
		update_account(account)
	elseif packet.result == -1 then --没有数据
		insert_account(account)
	else
		
	end
end

--===================================================
--新创建帐号
function insert_account(account)
	print("======insert_account==========")
	local value = {
		Account = account:AccountStr(),
		Lv = 1,
		Exp = 0,
		Gold = 100,
		VipLv = 1,
		VipExp = 0,
		LastLoginTime = G_GetSecond(),
		CreateTime = G_GetSecond(),
		DATA = "",
	}

	dbClient.command(dbClient.giCommandInsert, "account", value, insert_account_cb, account)
end

function insert_account_cb(packet, account)
	print("======insert_account_cb==========")
	update_account(account)
end

function update_account(account)
	print("======update_account==========")
	local value = {
		Account = account:AccountStr(),
		Lv = 1,
		Exp = 0,
		Gold = 100,
		VipLv = 1,
		VipExp = 0,
		LastLoginTime = G_GetSecond(),
		CreateTime = G_GetSecond(),
		DATA = "",
	}
	dbClient.command(dbClient.giCommandUpdate, "account", value, update_account_cb, account)
end

function update_account_cb(packet, account)
	print("======update_account_cb==========")
end
