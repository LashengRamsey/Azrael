rem 文件输出到target4pd
::pause
@echo off
for /f "delims=" %%a in ('dir/a-d/s/b *.proto') do (
::	ren "%%~a" "%%~na"
	echo %%~nxa
	echo %%~na
	protoc.exe -o%%~na.pd  %%~nxa
	
)
move /y *.pd .\target4pd\
pause