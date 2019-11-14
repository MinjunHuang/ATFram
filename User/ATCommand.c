#include "ATCommand.h"
#include "string.h"

char DateHandleBuff[128];

ATStatus AT_Callback(char* SendCommand,char * str)
{
	ATStatus RxHandleStatus;
	RxHandleStatus=CheckEcho(SendCommand,str);
	if(RxHandleStatus!=ATSUCCESS)
	{return ATERROR;}
	
	CheckEnd();
	return ATSUCCESS;
}

ATStatus CheckEcho(char* SendCommand,char * str)
{
	char *ret;
	int len=strlen(str);
	
	memcpy(DateHandleBuff,str,len);
	DateHandleBuff[len]=0;				//���ַ���ĩβ��0
	//���ҵ�һ�����з������л��Լ��
	ret = (char*)memchr(DateHandleBuff, '\n', strlen(str));
	
	*ret=0;				//�������ַ������нض�
	len=strlen(DateHandleBuff);			//���ظ��»����ַ�������
	
	//�������뷢���ַ��Ƿ�һ��
	if(strncmp(SendCommand, DateHandleBuff, len-1)==0)
	{
		__LOG("Data Consistency��Echo is:%s",DateHandleBuff);
		return ATSUCCESS;
	}
	else
	{
		__LOG("Inconsistency of Data��Echo is:%s%s",DateHandleBuff,SendCommand);
		__LOGARRAY(DateHandleBuff,len-1);
		__LOGARRAY(SendCommand,len-1);
		
		__ERRORLOG("������Բ�һ�£��������%s�����ջ��ԣ�%s",DateHandleBuff,SendCommand);
		return ATERROR;
	}
}

ATStatus CheckEnd(void)
{
	char *ret;
	int result;
	int len=strlen(DateHandleBuff);
	__LOG("DateHandleBuff[6] is:%s",&DateHandleBuff[6]);
	result=memcmp(&DateHandleBuff[len-6],"\r\nOK\r\n",6);
	__LOGARRAY(&DateHandleBuff[len-6],6);
	if(result==0)
	{
		__LOG("END SUCCESS OK");
		return ATSUCCESS;
	}
	
	__LOG("DateHandleBuff[9] is:%s",&DateHandleBuff[9]);
	result=memcmp(&DateHandleBuff[len-9],"\r\nERROR\r\n",9);
	if(result!=0)
	{
		__LOG("END SUCCESS ERROR");
		return ATERROR;
	}
	__LOG("END ERROR");
	return ATERROR;
}


ATStatus CGSN_Callback(char* SendCommand,char * str)
{
	return ATSUCCESS;
}




