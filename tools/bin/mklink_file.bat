@echo off
@rem 创建软连接文件( 名称 目标 )
set NAME=%~dp1
set TARGET=%~dp2
mklink   %NAME:/=\% %TARGET:/=\%