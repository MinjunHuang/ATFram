#include "ATCommand.h"
#include "string.h"

char DateHandleBuff[128];

ATStatus AT_Callback(char* SendCommand,char * str)
{
	
	return ATSUCCESS;
}

void CheckEcho(char* SendCommand,char * str)
{
	char *ret;
	int len=strlen(str);
	memcpy(DateHandleBuff,str,len);
	DateHandleBuff[len]=0;				//���ַ���ĩβ��0
	//���ҵ�һ�����з������л��Լ��
	ret = (char*)memchr(DateHandleBuff, '\n', strlen(str));
	*ret=0;								//���ַ���ĩβ��0		
	__LOG("Echo is:%s",DateHandleBuff);
}

ATStatus CGSN_Callback(char* SendCommand,char * str)
{
	return ATSUCCESS;
}




