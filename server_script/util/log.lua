
G_LOG_PATH = ""
function G_InitLog(path)
	G_LOG_PATH = string.format("%s.%s/", path, G_ServerNo)
end

function CLog(dir, str, ...)
	dir = string.format("%s%s", G_LOG_PATH, dir)
	str = string.format(str, ...)
	C_Log(dir, str)
end

function CLogInfo(dir, str, ...)
	dir = string.format("%s%s", G_LOG_PATH, dir)
	str = "[Info]" .. string.format(str, ...)
	print(str)
	C_Info(dir, str)
end

function CLogError(str, ...)
	dir = string.format("%s%s", G_LOG_PATH, "error")
	str = "[Error]" .. string.format(str, ...)
	print(str)
	C_Error(dir, str)
end

function CLogErrorDir(dir, str, ...)
	dir = string.format("%s%s", G_LOG_PATH, dir)
	str = "[Error]" .. string.format(str, ...)
	print(str)
	C_Error(dir, str)
end

--测试log
function G_TestLog()
	print("============G_TestLog=========")
	for i=1,100 do
		CLogInfo("TestLog/TestLog", "G_TestLog:%d", i)
	end
end

