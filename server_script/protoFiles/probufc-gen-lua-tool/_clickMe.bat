rem ================================================
rem 将../*proto生成.lua文件，生成的文件存放在outDir/
rem ================================================
set srcPath=..\proto\
set sourcefilter=*.proto
set destPath=..\target4lua\

for /d %%i in (%srcPath%\*) do (
	..\protoc.exe -I=%%i -I=%%i\.. --lua_out=%destPath% --plugin=protoc-gen-lua="protoc-gen-lua-tool.bat" %%i\*.proto
)

python makeService.py src=%srcPath% dest=%destPath% rpc=_rpc.lua require=_init.lua

move /y  "%destPath%"\*.lua  ..\..\pb\
rem 生成lua-proto成功!

pause


