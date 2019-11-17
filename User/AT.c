#include "AT.h"

QueueHandle_t 		ATcmdQueue = NULL;
SemaphoreHandle_t 	ATRXCplSemaphore ;

//AT��ܳ�ʼ��
ATStatus ATFormInit(void)
{
	ATcmdQueue = xQueueCreate(10, sizeof(ATCommandRegInfo));
	ATRXCplSemaphore = xSemaphoreCreateBinary();
	CheckATCmdConsistency();
	UartInit();
	
    if( ATcmdQueue == 0 )
    {
        /* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
		__ERRORLOG("ATcmdQueue ����ʧ��");
		while(1);
    }
	
	return ATSUCCESS;
}

//��������������������Ƿ�һ��
ATStatus CheckATCmdConsistency(void)
{
	for(int i=0;i<MAXCMDNUM;i++)
	{
		if(ATCommandList[i].ATCommandName!=(ATCommand)i)
		{
			__ERRORLOG("AT�����������ýṹ�岻һ�£�%d",i);
			return ATERROR;
		}
	}
	return ATSUCCESS;
}

//ע��AT����
void ATCommandRegister(ATCommand ATCommandName,CmdType ATCommandType,char* ATCommandParam)
{
	static int count=0;
	ATCommandRegInfo RegcommandInfo;
	RegcommandInfo.RegCommandName=ATCommandName;
	RegcommandInfo.RegCommandType=ATCommandType;
	RegcommandInfo.RegCommandParam=ATCommandParam;
	if( xQueueSend(ATcmdQueue,(void *) &RegcommandInfo,(TickType_t)100) != pdPASS )
	{/* ����ʧ��*/
	  __ERRORLOG("����������ʧ��,��������");
	}
	__LOG("ע�����:%d",count++);
	__LOGNOLF("Register CMD��%s",ATCommandList[ATCommandName].ATStr);
}


ATStatus CheckEcho(ATCommandRegInfo SendATInfo,char * str,char ** ReStr)
{
	char *ret;
	int Marklen=strlen(ATCommandList[SendATInfo.RegCommandName].ATStr);
	int result=1;
	int len =strlen(str);
	//��ȡ�����ַ�������
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
//	__LOGARRAY(str,len+1,"���յ����ַ���");

	//���ҵ�һ�����з������л��Լ��
	ret = (char*)memchr(str, '\n', len);
	*ReStr=ret+1;
	//�������뷢���ַ��Ƿ�һ��
	result=strncmp(ATCommandList[SendATInfo.RegCommandName].ATStr, str, Marklen);

	if(result==0)
	{
		*ret=0;	//�������ַ������нض�
		__LOG("���Լ����ɣ�Echo is:%s",str);

		return ATSUCCESS;
	}
	else
	{
		__ERRORLOG(" ������������Բ�һ�� ");
		__LOGARRAY(str,ret-str,"���յĻ���");
		__LOGARRAY(ATCommandList[SendATInfo.RegCommandName].ATStr,Marklen,"���͵�����");
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
//	__LOGARRAY(&str[Marklen-4],4+1,"��β�ַ���");
	if(result==0)
	{
		str[Marklen-6]=0;
		__LOG("SUCCESS END");
		return ATSUCCESS;
	}
	
	__LOGNOLF("DateHandleBuff[len-9] is:%s",&str[Marklen-7]);
	result=memcmp(&str[Marklen-7],"ERROR\r\n",7);
	__LOGARRAY(&str[Marklen-7],7+1,"��β�ַ���");
	if(result!=0)
	{
		str[Marklen-9]=0;
		__LOG("SUCCESS END ��REC ERROR");
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
  
  //���еȴ�����
  xResult = xQueueReceive(ATcmdQueue,  (void *)&SendATInfo, (TickType_t)portMAX_DELAY);
  if(xResult != pdPASS)
  {
  	__ERRORLOG("��ȡ��������ʧ��");
  }

  //����ATָ����ݴ���
  int ATMaxResponseTime	= ATCommandList[SendATInfo.RegCommandName].MaxResponseTime;
  int ATRetryCount		= ATCommandList[SendATInfo.RegCommandName].MaxTryCount;
  int ATResetCount		= ATCommandList[SendATInfo.RegCommandName].MaxResetCount;
  char *ATSendCommand	= ATCommandList[SendATInfo.RegCommandName].ATStr;
  char *ATSendParam		= SendATInfo.RegCommandParam;
  for(int j=0;j<ATResetCount;j++)
  {	//����ʧ�ܺ�����
  	for(int i=0;i<ATRetryCount;i++)
  	{//����ʧ�ܺ�����
  		__LOG("---------------------------------------------------------");
  		__LOG("���ʹ���:%d",i);
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
				__ERRORLOG("δ֪��������");
			}	break;
		}
  
  		//�ȴ��жϽ���������
  		xResult=xSemaphoreTake(ATRXCplSemaphore,pdMS_TO_TICKS(ATMaxResponseTime));
  		if(xResult != pdPASS)
  		{	//�ȴ����ݳ�ʱ�����·�����һ������
  			__ERRORLOG("�ȴ����ݳ�ʱ�����·�����һ������");
  			continue;
  		}
  		
  		//����������ݴ���ص�����
  		//������
  		char * ReStr;
  		if(CheckEcho(SendATInfo,UartRXBuff,&ReStr)!=ATSUCCESS)
  		{continue;}
  		
  		//������״̬
  		if(CheckEnd(ReStr)!=ATSUCCESS)
  		{continue;}
  		
  		if(ATCommandList[SendATInfo.RegCommandName].ATRxCpltCallback!=NULL)
  		{
			__LOGNOLF("���ղ���Ϊ:%s",ReStr);
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
  	**	_BUG_	�������⣬���޸�
  	**	ʹ��ģ����������������ģ��ֱ��ִ���ϴ�ʧ������
  	**	�����������Ҫ��������������ã���ʼ��ʧ��
  	*/
  	MoudleResst();
  	//__LOG("ָ��:%s���մ���,��������:%d\n",SendATConfig.ATStr,j+1);
  }

}

