@echo off
set PROJECT_PATH=%~dp0
cd /d %PROJECT_PATH%
call %PROJECT_PATH%\config.bat

echo RTTHREADDIR=%RTTHREADDIR% RTTHREADBSPDIR=%RTTHREADBSPDIR%
set  RTTHREADDIR_PATH=%RTTHREADDIR:/=\%
set  RTTHREADBSPDIR_PATH=%RTTHREADBSPDIR:/=\%

cd /d %RTTHREADBSPDIR_PATH%

cmd /c menuconfig
cmd /c pkgs --update
cmd /c scons --target=mdk5

cd /d %PROJECT_PATH%

copy /Y /V %RTTHREADBSPDIR_PATH%\.config .config
copy /Y /V %RTTHREADBSPDIR_PATH%\rtconfig.h rtconfig.h

goto Eof
:Error
exit /b -1
:Eof
exit /b 0