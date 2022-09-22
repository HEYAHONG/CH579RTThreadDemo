@echo off
@rem 创建软连接目录( 名称 目标 )
set NAME=%~dp1
set TARGET=%~dp2
mklink /d /j  %NAME:/=\%  %TARGET:/=\%