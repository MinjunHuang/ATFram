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
	__LOGARRAY(DateHandleBuff,RxHandleInfo.len+1,"接收到的字符串");

	//查找第一个换行符，进行回显检查
	ret = (char*)memchr(DateHandleBuff, '\n', RxHandleInfo.len);

	//检查回显与发送字符是否一致
	result=strncmp(SendCommand, DateHandleBuff, Marklen);
				
	
	if(result==0)
	{
		*ret=0;	//将回显字符串进行截断
		__LOG("回显检查完成，Echo is:%s",DateHandleBuff);
		return ATSUCCESS;
	}
	else
	{
		__ERRORLOG(" 发送命令与回显不一致 ");
		__LOGARRAY(DateHandleBuff,ret-DateHandleBuff,"接收的回显");
		__LOGARRAY(SendCommand,Marklen,"发送的命令");
		
		
		return ATERROR;
	}
}

ATStatus CheckEnd(void)
{
	int result;
	int Marklen=strlen(DateHandleBuff);
	
	__LOG("DateHandleBuff[len-4] is:%s",&DateHandleBuff[RxHandleInfo.len-4]);
	
	result=memcmp(&DateHandleBuff[RxHandleInfo.len-4],"OK\r\n",4);
	__LOGARRAY(&DateHandleBuff[RxHandleInfo.len-4],4+1,"结尾字符串");
	if(result==0)
	{
		__LOG("SUCCESS END");
		return ATSUCCESS;
	}
	
	__LOG("DateHandleBuff[len-7] is:%s",&DateHandleBuff[RxHandleInfo.len-7+1]);
	result=memcmp(&DateHandleBuff[RxHandleInfo.len-7],"\r\nERROR\r\n",7+1);
	if(result!=0)
	{
		__LOG("SUCCESS END ，REC ERROR");
		return ATERROR;
	}
	
	__LOG("ERROR  END");
	return ATERROR;
}

ATStatus AT_Callback(char* SendCommand,char * str)
{
	//检查回显
	if(CheckEcho(SendCommand,str)!=ATSUCCESS)
	{return ATERROR;}
	
	//检查结束状态
	if(CheckEnd()!=ATSUCCESS)
	{return ATERROR;}
	
	return ATSUCCESS;
}

ATStatus CGSN_Callback(char* SendCommand,char * str)
{
	//检查回显
	if(CheckEcho(SendCommand,str)!=ATSUCCESS)
	{return ATERROR;}
	
	//检查结束状态
	if(CheckEnd()!=ATSUCCESS)
	{return ATERROR;}
	
	
	return ATSUCCESS;
}




