@echo off
set PROJECT_PATH=%~dp0
cd /d %PROJECT_PATH%
set PROJECT_PATH_FIX=%PROJECT_PATH:\=/%
set PATH=%PROJECT_PATH%\tools\bin;%PATH%

set RTTHREADDIR=%PROJECT_PATH_FIX%/rt-thread
set RTTHREADBSPDIR=%PROJECT_PATH_FIX%/rt-thread/bsp/wch/arm/ch579m