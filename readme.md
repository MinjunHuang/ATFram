# AT指令处理框架
AT指令处理框架，基于FreeRTOS，使用STM32进行开发

---

1. 在使用前将Hardware.c 与 ATCommand.h ATCommand.c中的函数进行配置。
2. 使用时注册一个任务运行AT指令处理调度器，使用ATCommandRegister()函数注册发送AT命令。当接收到正确的响应后，会将参数传递到用户的处理回调函数中。


## 提交版本日志：
1. 11-6 完成AT指令发送注册和返回命令参数提取

## 代码结构
```
	├── ATFramework.ioc  	//CUBEMX工程
	├── Drivers				//HAL库驱动
	├── Inc					//工程头文件
	├── MDK-ARM				//keil工程文件
	├── Middlewares			//FreeRTOS库
	├── Src					//工程源文件
	├── User				//AT驱动库文件
	└── readme.md			

```

## 参考链接：
1. [AT指令代码与实现方法（基于C语言）](https://blog.csdn.net/hnxyxiaomeng/article/details/84613712)
2. [STM32 HAL库学习（四）：DMA之串口空闲中断](https://blog.csdn.net/la_fe_/article/details/100543141)
