#include "AT.h"

QueueHandle_t 		ATcmdQueue = NULL;
SemaphoreHandle_t 	ATRXCplSemaphore ;

//AT框架初始化
ATStatus ATFormInit(void)
{
	ATcmdQueue = xQueueCreate(10, sizeof(ATCommandRegInfo));
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
		if(ATCommandList[i].ATCommandName!=(ATCommand)i)
		{
			__ERRORLOG("AT命令名与配置结构体不一致，%d",i);
			return ATERROR;
		}
	}
	return ATSUCCESS;
}

//注册AT命令
void ATCommandRegister(ATCommand ATCommandName,CmdType ATCommandType,char* ATCommandParam)
{
	static int count=0;
	ATCommandRegInfo RegcommandInfo;
	RegcommandInfo.RegCommandName=ATCommandName;
	RegcommandInfo.RegCommandType=ATCommandType;
	RegcommandInfo.RegCommandParam=ATCommandParam;
	if( xQueueSend(ATcmdQueue,(void *) &RegcommandInfo,(TickType_t)100) != pdPASS )
	{/* 发送失败*/
	  __ERRORLOG("添加命令到队列失败,队列已满");
	}
	__LOG("注册次数:%d",count++);
	__LOGNOLF("Register CMD：%s",ATCommandList[ATCommandName].ATStr);
}


ATStatus CheckEcho(ATCommandRegInfo SendATInfo,char * str,char ** ReStr)
{
	char *ret;
	int Marklen=strlen(ATCommandList[SendATInfo.RegCommandName].ATStr);
	int result=1;
	int len =strlen(str);
	//获取发送字符串长度
//	switch((int)SendATInfo.RegCommandType)
//	{
//		case EXEXCMD:		//AT+<cmd>
//			Marklen=strlen(ATCommandList[SendATInfo.RegCommandName].ATStr)+strlen(SendATInfo.RegCommandParam);
//		break;
//		case READCMD:		//AT+<cmd>? 
//			Marklen=strlen(ATCommandList[SendATInfo.RegCommandName].ATStr)+1;
//		break;
//		default:			//AT+<cmd>=<...> 
//			Marklen=strlen(ATCommandList[SendATInfo.RegCommandName].ATStr)+strlen(SendATInfo.RegCommandParam)+1;
//		break;
//	}
//	__LOGARRAY(str,len+1,"接收到的字符串");

	//查找第一个换行符，进行回显检查
	ret = (char*)memchr(str, '\n', len);
	*ReStr=ret+1;
	//检查回显与发送字符是否一致
	result=strncmp(ATCommandList[SendATInfo.RegCommandName].ATStr, str, Marklen);

	if(result==0)
	{
		*ret=0;	//将回显字符串进行截断
		__LOG("回显检查完成，Echo is:%s",str);

		return ATSUCCESS;
	}
	else
	{
		__ERRORLOG(" 发送命令与回显不一致 ");
		__LOGARRAY(str,ret-str,"接收的回显");
		__LOGARRAY(ATCommandList[SendATInfo.RegCommandName].ATStr,Marklen,"发送的命令");
		ReStr=NULL;
		return ATERROR;
	}
}

ATStatus CheckEnd(char *str)
{
	int result;
	int Marklen=strlen(str);

	__LOGNOLF("DateHandleBuff[len-6] is:%s",&str[Marklen-4]);
	result=memcmp(&str[Marklen-4],"OK\r\n",4);
//	__LOGARRAY(&str[Marklen-4],4+1,"结尾字符串");
	if(result==0)
	{
		str[Marklen-6]=0;
		__LOG("SUCCESS END");
		return ATSUCCESS;
	}
	
	__LOGNOLF("DateHandleBuff[len-9] is:%s",&str[Marklen-7]);
	result=memcmp(&str[Marklen-7],"ERROR\r\n",7);
	__LOGARRAY(&str[Marklen-7],7+1,"结尾字符串");
	if(result!=0)
	{
		str[Marklen-9]=0;
		__LOG("SUCCESS END ，REC ERROR");
		return ATERROR;
	}
	
	__LOG("ERROR  END");
	return ATERROR;
}

void ATCommandSendScheduler(void)
{
  BaseType_t xResult;
  ATStatus ATResult;
  ATCommandRegInfo SendATInfo;
  
  //队列等待数据
  xResult = xQueueReceive(ATcmdQueue,  (void *)&SendATInfo, (TickType_t)portMAX_DELAY);
  if(xResult != pdPASS)
  {
  	__ERRORLOG("读取队列数据失败");
  }

  //发送AT指令并数据处理
  int ATMaxResponseTime	= ATCommandList[SendATInfo.RegCommandName].MaxResponseTime;
  int ATRetryCount		= ATCommandList[SendATInfo.RegCommandName].MaxTryCount;
  int ATResetCount		= ATCommandList[SendATInfo.RegCommandName].MaxResetCount;
  char *ATSendCommand	= ATCommandList[SendATInfo.RegCommandName].ATStr;
  char *ATSendParam		= SendATInfo.RegCommandParam;
  for(int j=0;j<ATResetCount;j++)
  {	//发送失败后重启
  	for(int i=0;i<ATRetryCount;i++)
  	{//发送失败后重试
  		__LOG("---------------------------------------------------------");
  		__LOG("发送次数:%d",i);
  		switch((int)SendATInfo.RegCommandType)
		{
			case EXEXCMD:{
				SendMultiStr(3,ATSendCommand,ATSendParam,"\r\n");
			}break;
			case READCMD:{
				SendMultiStr(4,ATSendCommand,"?",ATSendParam,"\r\n");
			}break;
			case WRITECMD:{
				SendMultiStr(4,ATSendCommand,"=",ATSendParam,"\r\n");
			}break;
			default:{
				__ERRORLOG("未知命令类型");
			}	break;
		}
  
  		//等待中断接收完数据
  		xResult=xSemaphoreTake(ATRXCplSemaphore,pdMS_TO_TICKS(ATMaxResponseTime));
  		if(xResult != pdPASS)
  		{	//等待数据超时，重新发起下一次请求
  			__ERRORLOG("等待数据超时，重新发起下一次请求");
  			continue;
  		}
  		
  		//进入接收数据处理回调函数
  		//检查回显
  		char * ReStr;
  		if(CheckEcho(SendATInfo,UartRXBuff,&ReStr)!=ATSUCCESS)
  		{continue;}
  		
  		//检查结束状态
  		if(CheckEnd(ReStr)!=ATSUCCESS)
  		{continue;}
  		
  		if(ATCommandList[SendATInfo.RegCommandName].ATRxCpltCallback!=NULL)
  		{
			__LOGNOLF("接收参数为:%s",ReStr);
  			ATResult=ATCommandList[SendATInfo.RegCommandName].ATRxCpltCallback(ReStr);
  			if(ATResult==ATSUCCESS)
  			{return;}
  		}else
  		{
  			return;
  		}
  		
  		
  	}
  	
  	__LOG("******************************************************");
  	
  	/*
  	**	_BUG_	发现问题，待修复
  	**	使用模块重启，当重启后模块直接执行上次失败命令
  	**	如果该命令需要其他命令进行配置，将始终失败
  	*/
  	MoudleResst();
  	//__LOG("指令:%s接收错误,重启次数:%d\n",SendATConfig.ATStr,j+1);
  }

}

