@echo off
set PROJECT_PATH=%~dp0
cd /d %PROJECT_PATH%
set PROJECT_PATH_FIX=%PROJECT_PATH:\=/%
set PATH=%PROJECT_PATH%\tools\bin;%PATH%
%PROJECT_PATH%\tools\bin\bash.exe  %PROJECT_PATH_FIX%/bootstrap