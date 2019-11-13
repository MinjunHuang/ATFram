#include "AT.h"

ATStatus ATFormInit(void)
{
	CheckATCmdConsistency();
	
	return ATSUCCESS;
}

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

void ATCommandRegister(ATCommandConfig *ATCommamd)
{
	if( xQueueSend(ATcmdQueue,(void *) &ATCommamd,(TickType_t)100) != pdPASS )
	{
	  /* 发送失败*/
	  __ERRORLOG("添加命令到队列失败,队列已满");
	}
}



