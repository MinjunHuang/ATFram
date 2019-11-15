#include "AT.h"

QueueHandle_t 		ATcmdQueue = NULL;
SemaphoreHandle_t 	ATRXCplSemaphore ;

//AT框架初始化
ATStatus ATFormInit(void)
{
	ATcmdQueue = xQueueCreate(10, sizeof(ATCommandConfig));
	ATRXCplSemaphore = xSemaphoreCreateBinary();
	CheckATCmdConsistency();
	UartInit();
	
    if( ATcmdQueue == 0 )
    {
        /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
		__ERRORLOG("ATcmdQueue 创建失败");
		while(1);
    }
	
	return ATSUCCESS;
}

//检查命令名和命令配置是否一致
ATStatus CheckATCmdConsistency(void)
{
	for(int i=0;i<MAXCMDNUM;i++)
	{
		if(ATCommandList[i].ATCommandName!=(eATCommand)i)
		{
			__ERRORLOG("AT命令名与配置结构体不一致，%d",i);
			return ATERROR;
		}
	}
	return ATSUCCESS;
}

//注册AT命令
void ATCommandRegister(eATCommand	ATCommandName)
{
	if( xQueueSend(ATcmdQueue,(void *) &ATCommandList[ATCommandName],(TickType_t)100) != pdPASS )
	{/* 发送失败*/
	  __ERRORLOG("添加命令到队列失败,队列已满");
	}
	__LOG("Register CMD：%s",ATCommandList[ATCommandName].ATStr);
}

void ATCommandSendScheduler(void)
{
	BaseType_t xResult;
	ATCommandConfig SendATConfig;
	ATStatus ATResult;
	
	//队列等待数据
	xResult = xQueueReceive(ATcmdQueue,  (void *)&SendATConfig, (TickType_t)portMAX_DELAY);
	if(xResult != pdPASS)
	{
		__ERRORLOG("读取队列数据失败");
	}
	
	//发送AT指令并数据处理
	for(int j=0;j<SendATConfig.MaxResetCount;j++)
	{	//发送失败后重启
		for(int i=0;i<SendATConfig.MaxTryCount;i++)
		{//发送失败后重试
			
			SendString(SendATConfig.ATStr);
			//等待中断接收完一帧数据

			//等待中断接收完数据
			xResult=xSemaphoreTake(ATRXCplSemaphore,pdMS_TO_TICKS(SendATConfig.MaxResponseTime));
			if(xResult != pdPASS)
			{	//等待数据超时，重新发起下一次请求
				__ERRORLOG("等待数据超时，重新发起下一次请求");
				break;
			}
			
			//进入接收数据处理回调函数
			ATResult=SendATConfig.ATRxCpltCallback(SendATConfig.ATStr,UartRXBuff);
			if(ATResult==ATSUCCESS)
			{
				break;
			}
			__LOG("指令:%s，状态错误,重试次数:%d\n",SendATConfig.ATStr,i+1);
		}
		
		if(ATResult==ATSUCCESS)
		{//数据处理完成
			break;
		}
		__LOG("指令:%s接收错误,重启次数:%d\n",SendATConfig.ATStr,j);
	}

}

