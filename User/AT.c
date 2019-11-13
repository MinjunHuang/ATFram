#include "AT.h"

QueueHandle_t ATcmdQueue = NULL;

//AT框架初始化
ATStatus ATFormInit(void)
{
	ATcmdQueue = xQueueCreate(10, sizeof(ATCommandConfig));
	
	CheckATCmdConsistency();
	UartInit();
	
    if( ATcmdQueue == 0 )
    {
        /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
		__ERRORLOG("ATcmdQueue 创建失败");
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
	
	xResult = xQueueReceive(ATcmdQueue,  (void *)&SendATConfig, (TickType_t)portMAX_DELAY);
	if(xResult != pdPASS)
	{
		__ERRORLOG("读取队列数据失败");
	}
	
	//发送失败后重试
	for(int i=0;i<SendATConfig.MaxTryCount;i++)
	{
		SendString(SendATConfig.ATStr);
		ATResult=SendATConfig.ATRxCpltCallback(UartRXBuff);
		if(ATResult==ATSUCCESS)
		{
			break;
		}
	}
	
	
}

