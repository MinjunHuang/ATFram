#include "AT.h"

QueueHandle_t ATcmdQueue = NULL;

//AT��ܳ�ʼ��
ATStatus ATFormInit(void)
{
	ATcmdQueue = xQueueCreate(10, sizeof(ATCommandConfig));
	
	CheckATCmdConsistency();
	UartInit();
	
    if( ATcmdQueue == 0 )
    {
        /* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
		__ERRORLOG("ATcmdQueue ����ʧ��");
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
	__LOG("Register CMD��%s",ATCommandList[ATCommandName].ATStr);
}

void ATCommandSendScheduler(void)
{
	BaseType_t xResult;
	ATCommandConfig SendATConfig;
	ATStatus ATResult;
	
	xResult = xQueueReceive(ATcmdQueue,  (void *)&SendATConfig, (TickType_t)portMAX_DELAY);
	if(xResult != pdPASS)
	{
		__ERRORLOG("��ȡ��������ʧ��");
	}
	
	//����ʧ�ܺ�����
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

