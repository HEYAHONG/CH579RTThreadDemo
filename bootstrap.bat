@echo off
set PROJECT_PATH=%~dp0
cd /d %PROJECT_PATH%
call %PROJECT_PATH%\config.bat

%PROJECT_PATH%\tools\bin\bash.exe  %PROJECT_PATH_FIX%/bootstrap