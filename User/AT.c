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
			__ERRORLOG("AT�����������ýṹ�岻һ�£�%d",i);
			return ATERROR;
		}
	}
	return ATSUCCESS;
}

void ATCommandRegister(ATCommandConfig *ATCommamd)
{
	if( xQueueSend(ATcmdQueue,(void *) &ATCommamd,(TickType_t)100) != pdPASS )
	{
	  /* ����ʧ��*/
	  __ERRORLOG("����������ʧ��,��������");
	}
}



