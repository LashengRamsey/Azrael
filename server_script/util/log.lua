
function CLog(str, ...)
	local s = string.format(str, ...)
	C_Log(s)
end

function CLogInfo(str, ...)
	local s = string.format(str, ...)
	C_Info(s)
end

function CLogError(str, ...)
	local s = string.format(str, ...)
	C_Error(s)
end

