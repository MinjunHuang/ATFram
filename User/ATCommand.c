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
	int Marklen;
	RxHandleInfo.len =strlen(str);
	memcpy(DateHandleBuff,str,RxHandleInfo.len+1);
	RxHandleInfo.RxBuff=DateHandleBuff;
	__LOGARRAY(DateHandleBuff,RxHandleInfo.len+1,"���յ����ַ���");

	//���ҵ�һ�����з������л��Լ��
	ret = (char*)memchr(DateHandleBuff, '\n', strlen(str));
	*ret=0;				//�������ַ������нض�
	__LOGARRAY(DateHandleBuff,RxHandleInfo.len+1,"�����ַ���Ƭ");
	Marklen=strlen(DateHandleBuff);			//���»����ַ�������
	
	//�������뷢���ַ��Ƿ�һ��
	if(strncmp(SendCommand, DateHandleBuff, Marklen-1)==0)
	{
		__LOG("Data Consistency��Echo is:%s",DateHandleBuff);
		return ATSUCCESS;
	}
	else
	{
		__LOG("Inconsistency of Data��Echo is:%s%s",DateHandleBuff,SendCommand);
		__LOGARRAY(DateHandleBuff,Marklen-1,"���յ��Ļ���");
		__LOGARRAY(SendCommand,Marklen-1,"���͵�����");
		
		__ERRORLOG("������Բ�һ�£��������%s�����ջ��ԣ�%s",DateHandleBuff,SendCommand);
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




