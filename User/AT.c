#include "AT.h"

QueueHandle_t 		ATcmdQueue = NULL;
SemaphoreHandle_t 	ATRXCplSemaphore ;

//AT��ܳ�ʼ��
ATStatus ATFormInit(void)
{
	ATcmdQueue = xQueueCreate(10, sizeof(ATCommandConfig));
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
		if(ATCommandList[i].ATCommandName!=(eATCommand)i)
		{
			__ERRORLOG("AT�����������ýṹ�岻һ�£�%d",i);
			return ATERROR;
		}
	}
	return ATSUCCESS;
}

//ע��AT����
void ATCommandRegister(eATCommand	ATCommandName)
{
	if( xQueueSend(ATcmdQueue,(void *) &ATCommandList[ATCommandName],(TickType_t)100) != pdPASS )
	{/* ����ʧ��*/
	  __ERRORLOG("����������ʧ��,��������");
	}
	
	__LOGNOLF("Register CMD��%s",ATCommandList[ATCommandName].ATStr);
}


ATStatus CheckEcho(char* SendCommand,char * str,char ** ReStr)
{
	char *ret;
	int Marklen=strlen(SendCommand);
	int result=1;
	int len =strlen(str);
	
//	__LOGARRAY(str,len+1,"���յ����ַ���");

	//���ҵ�һ�����з������л��Լ��
	ret = (char*)memchr(str, '\n', len);
	*ReStr=ret+1;
	//�������뷢���ַ��Ƿ�һ��
	result=strncmp(SendCommand, str, Marklen);

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
		__LOGARRAY(SendCommand,Marklen,"���͵�����");
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
  ATCommandConfig SendATConfig;
  ATStatus ATResult;
  
  //���еȴ�����
  xResult = xQueueReceive(ATcmdQueue,  (void *)&SendATConfig, (TickType_t)portMAX_DELAY);
  if(xResult != pdPASS)
  {
  	__ERRORLOG("��ȡ��������ʧ��");
  }
  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_RESET);
  //����ATָ����ݴ���
  for(int j=0;j<SendATConfig.MaxResetCount;j++)
  {	//����ʧ�ܺ�����
  	for(int i=0;i<SendATConfig.MaxTryCount;i++)
  	{//����ʧ�ܺ�����
  		
  		__LOG("---------------------------------------------------------");
  		__LOG("���ʹ���:%d",i);
  		
  		SendString(SendATConfig.ATStr);
  		//�ȴ��жϽ�����һ֡����
  
  		//�ȴ��жϽ���������
  		xResult=xSemaphoreTake(ATRXCplSemaphore,pdMS_TO_TICKS(SendATConfig.MaxResponseTime));
  		if(xResult != pdPASS)
  		{	//�ȴ����ݳ�ʱ�����·�����һ������
  			__ERRORLOG("�ȴ����ݳ�ʱ�����·�����һ������");
  			continue;
  		}
  		
  		//����������ݴ���ص�����
  		//������
  		char * ReStr;
  		if(CheckEcho(SendATConfig.ATStr,UartRXBuff,&ReStr)!=ATSUCCESS)
  		{continue;}
  		
  		//������״̬
  		if(CheckEnd(ReStr)!=ATSUCCESS)
  		{continue;}
  		
  		if(SendATConfig.ATRxCpltCallback!=NULL)
  		{
			__LOGNOLF("���ղ���Ϊ:%s",ReStr);
  			ATResult=SendATConfig.ATRxCpltCallback(ReStr);
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
  	__LOG("ָ��:%s���մ���,��������:%d\n",SendATConfig.ATStr,j+1);
  }

}

