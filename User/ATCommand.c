#include "ATCommand.h"
#include "string.h"

typedef struct 
{
	char* RxBuff;
	int   len;
}RxStrInfo;

char DateHandleBuff[128];
RxStrInfo RxHandleInfo;


ATStatus CheckEcho(char* SendCommand,char * str)
{
	char *ret;
	int Marklen=strlen(SendCommand);
	int result=1;
	RxHandleInfo.len =strlen(str);
	
	memcpy(DateHandleBuff,str,RxHandleInfo.len+1);
	RxHandleInfo.RxBuff=DateHandleBuff;
	__LOGARRAY(DateHandleBuff,RxHandleInfo.len+1,"���յ����ַ���");

	//���ҵ�һ�����з������л��Լ��
	ret = (char*)memchr(DateHandleBuff, '\n', RxHandleInfo.len);

	//�������뷢���ַ��Ƿ�һ��
	result=strncmp(SendCommand, DateHandleBuff, Marklen);
				
	
	if(result==0)
	{
		*ret=0;	//�������ַ������нض�
		__LOG("���Լ����ɣ�Echo is:%s",DateHandleBuff);
		return ATSUCCESS;
	}
	else
	{
		__ERRORLOG(" ������������Բ�һ�� ");
		__LOGARRAY(DateHandleBuff,ret-DateHandleBuff,"���յĻ���");
		__LOGARRAY(SendCommand,Marklen,"���͵�����");
		
		
		return ATERROR;
	}
}

ATStatus CheckEnd(void)
{
	int result;
	int Marklen=strlen(DateHandleBuff);
	
	__LOG("DateHandleBuff[len-4] is:%s",&DateHandleBuff[RxHandleInfo.len-4]);
	
	result=memcmp(&DateHandleBuff[RxHandleInfo.len-4],"OK\r\n",4);
	__LOGARRAY(&DateHandleBuff[RxHandleInfo.len-4],4+1,"��β�ַ���");
	if(result==0)
	{
		__LOG("SUCCESS END");
		return ATSUCCESS;
	}
	
	__LOG("DateHandleBuff[len-7] is:%s",&DateHandleBuff[RxHandleInfo.len-7+1]);
	result=memcmp(&DateHandleBuff[RxHandleInfo.len-7],"\r\nERROR\r\n",7+1);
	if(result!=0)
	{
		__LOG("SUCCESS END ��REC ERROR");
		return ATERROR;
	}
	
	__LOG("ERROR  END");
	return ATERROR;
}

ATStatus AT_Callback(char* SendCommand,char * str)
{
	//������
	if(CheckEcho(SendCommand,str)!=ATSUCCESS)
	{return ATERROR;}
	
	//������״̬
	if(CheckEnd()!=ATSUCCESS)
	{return ATERROR;}
	
	return ATSUCCESS;
}

ATStatus CGSN_Callback(char* SendCommand,char * str)
{
	//������
	if(CheckEcho(SendCommand,str)!=ATSUCCESS)
	{return ATERROR;}
	
	//������״̬
	if(CheckEnd()!=ATSUCCESS)
	{return ATERROR;}
	
	
	return ATSUCCESS;
}




