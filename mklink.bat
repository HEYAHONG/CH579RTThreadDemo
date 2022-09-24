@echo off
set PROJECT_PATH=%~dp0
cd /d %PROJECT_PATH%
call %PROJECT_PATH%\config.bat

@REM 提权
PUSHD "%~dp0" & set "ROOT=%~dp0" & REG.EXE QUERY "HKU\S-1-5-19">NUL 2>&1
IF %ERRORLEVEL% EQU 1 ( MSHTA "vbscript:CreateObject("Shell.Application").ShellExecute("%~fs0", "", "", "runas", 1) & Close()" & EXIT /B )


echo RTTHREADDIR=%RTTHREADDIR% RTTHREADBSPDIR=%RTTHREADBSPDIR%
set  RTTHREADDIR_PATH=%RTTHREADDIR:/=\%
set  RTTHREADBSPDIR_PATH=%RTTHREADBSPDIR:/=\%

mklink /D /J %RTTHREADBSPDIR_PATH%\src .\
mklink project.uvprojx  %RTTHREADBSPDIR_PATH%\project.uvprojx

goto Eof
:Error
exit -1
:Eof
exit 0