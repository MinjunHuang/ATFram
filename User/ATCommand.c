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
	__LOGARRAY(DateHandleBuff,RxHandleInfo.len+1,"接收到的字符串");

	//查找第一个换行符，进行回显检查
	ret = (char*)memchr(DateHandleBuff, '\n', strlen(str));
	*ret=0;				//将回显字符串进行截断
	__LOGARRAY(DateHandleBuff,RxHandleInfo.len+1,"回显字符切片");
	Marklen=strlen(DateHandleBuff);			//更新回显字符串长度
	
	//检查回显与发送字符是否一致
	if(strncmp(SendCommand, DateHandleBuff, Marklen-1)==0)
	{
		__LOG("Data Consistency，Echo is:%s",DateHandleBuff);
		return ATSUCCESS;
	}
	else
	{
		__LOG("Inconsistency of Data，Echo is:%s%s",DateHandleBuff,SendCommand);
		__LOGARRAY(DateHandleBuff,Marklen-1,"接收到的回显");
		__LOGARRAY(SendCommand,Marklen-1,"发送的命令");
		
		__ERRORLOG("命令回显不一致，发送命令：%s，接收回显：%s",DateHandleBuff,SendCommand);
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




