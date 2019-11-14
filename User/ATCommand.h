#ifndef _ATCOMMAND_H_
#define _ATCOMMAND_H_

#include "ATCommand.h"
#include "stdint.h"
#include "log.h"

typedef enum 
{
	ATERROR = 0, 
	ATSUCCESS ,
	ATERRORCODE1,
}ATStatus;

typedef ATStatus (*pFuncCallback)(char* SendCommand,char* str);

ATStatus AT_Callback(char* SendCommand,char * str);
ATStatus CGSN_Callback(char* SendCommand,char * str);

ATStatus CheckEcho(char* SendCommand,char * str);
ATStatus CheckEnd(void);

typedef enum 
{
	EXEXCMD,
	TESTCMD,
	READCMD,
	WRITECMD,
}CmdType;

typedef enum 
{
	AT=0,
	CGSN,
	
	/**��ָ����ӵ�����**/
	MAXCMDNUM
}eATCommand;

typedef struct
{		
	eATCommand	ATCommandName;
	char * 		ATStr;				//���͵�ATָ��
	CmdType		ATCommandType;		//��������
	uint16_t  	MaxResponseTime;	//���ͺ��ѯ������Ϣ����ʱ��msΪ��λ������Ϊָ�������Ӧʱ�䡣
	uint8_t   	MaxTryCount; 		//������Դ���
	uint8_t   	MaxResetCount; 		//�����������
	pFuncCallback		ATRxCpltCallback;	//ATָ�������ɣ�ָ���ص�����
} ATCommandConfig;


static const ATCommandConfig ATCommandList[]=
{
	{AT,	"AT\r\n",	EXEXCMD,500,5,3,AT_Callback		},
	{CGSN,	"AT+CGSN=1\r\n",EXEXCMD,500,5,3,CGSN_Callback	},
};

#endif
