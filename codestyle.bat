@echo off
set PROJECT_PATH=%~dp0
cd /d %PROJECT_PATH%
call %PROJECT_PATH%\config.bat

astyle --style=allman --indent=spaces=4 --pad-oper --pad-header --unpad-paren --suffix=none --align-pointer=name --lineend=windows --convert-tabs  --recursive --ascii --verbose src\*.cpp
astyle --style=allman --indent=spaces=4 --pad-oper --pad-header --unpad-paren --suffix=none --align-pointer=name --lineend=windows --convert-tabs  --recursive --ascii --verbose src\*.hpp
astyle --style=allman --indent=spaces=4 --pad-oper --pad-header --unpad-paren --suffix=none --align-pointer=name --lineend=windows --convert-tabs  --recursive --ascii --verbose src\*.h
astyle --style=allman --indent=spaces=4 --pad-oper --pad-header --unpad-paren --suffix=none --align-pointer=name --lineend=windows --convert-tabs  --recursive --ascii --verbose src\*.c