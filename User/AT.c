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
	__LOG("Register CMD��%s",ATCommandList[ATCommandName].ATStr);
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
	
	//����ATָ����ݴ���
	for(int j=0;j<SendATConfig.MaxResetCount;j++)
	{	//����ʧ�ܺ�����
		for(int i=0;i<SendATConfig.MaxTryCount;i++)
		{//����ʧ�ܺ�����
			
			SendString(SendATConfig.ATStr);
			//�ȴ��жϽ�����һ֡����

			//�ȴ��жϽ���������
			xResult=xSemaphoreTake(ATRXCplSemaphore,pdMS_TO_TICKS(SendATConfig.MaxResponseTime));
			if(xResult != pdPASS)
			{	//�ȴ����ݳ�ʱ�����·�����һ������
				__ERRORLOG("�ȴ����ݳ�ʱ�����·�����һ������");
				break;
			}
			
			//����������ݴ���ص�����
			ATResult=SendATConfig.ATRxCpltCallback(SendATConfig.ATStr,UartRXBuff);
			if(ATResult==ATSUCCESS)
			{
				break;
			}
			__LOG("ָ��:%s��״̬����,���Դ���:%d\n",SendATConfig.ATStr,i+1);
		}
		
		if(ATResult==ATSUCCESS)
		{//���ݴ������
			break;
		}
		__LOG("ָ��:%s���մ���,��������:%d\n",SendATConfig.ATStr,j);
	}

}

