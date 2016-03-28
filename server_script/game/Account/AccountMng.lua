module("AccountMng", package.seeall)

local gAccountMngMap = {} or gAccountMngMap


function AccountMng:newAccount(sessionObj, sAccount)
	local account = gAccountMngMap[sAccount]
	if account then
		local oldSession = account:getSession()
		if oldSession then
			oldSession:setRefObj(nil)
		end
		account:setSession(sessionObj)
	else
		account = Account.Account:new(sessionObj, sAccount)
	end
	if sessionObj then
		sessionObj:setRefObj(account)
	end
	return account
end

function AccountMng:getAccount(sAccount)
	return gAccountMngMap[sAccount]
end

function AccountMng:delAccount(sAccount)
	gAccountMngMap[sAccount] = nil
end

