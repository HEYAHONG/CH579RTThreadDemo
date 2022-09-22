# 说明

这是一个使用[rt-thread](https://www.rt-thread.org/)开发Ch579m的例子(仅供测试)。

# 编译条件

- 安装好Rt-Thread Env工具。
- 安装好Keil5环境。
- 安装好设备支持包。[doc/CH579EVT/EVT/PUB/Keil.WCH57x_DFP.1.1.0.pack](./doc/CH579EVT/EVT/PUB/Keil.WCH57x_DFP.1.1.0.pack)
- 操作系统为Windows 7以上

# 编译

- 打开Rt-Thread Env工具(运行env.exe),切换至工程目录(即bootstrap.bat所在目录)。
- 运行bootstrap.bat,将产生project.uvprojx。
- 使用keil5打开project.uvprojx编译调试。

注意:添加源代码需要放在src目录中,每次更新了src目录都需要重新运行bootstrap.bat,在keil5添加文件只是临时的。

# 调试

可采用jlink直接调试，具体设置及问题参考lib/CH579EVT/中的例子。

## 串口

UART1 :115200 8N1