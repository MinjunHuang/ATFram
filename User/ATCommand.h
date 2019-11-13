#ifndef _ATCOMMAND_H_
#define _ATCOMMAND_H_

#include "stdint.h"

typedef void (*pFunc)(char* str);

typedef enum 
{
  ATERROR = 0, 
  ATSUCCESS = !ATERROR
}ATStatus;

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
	uint8_t   	RetryDelay; 		//����ʧ�ܺ��ٴη���ʱ����ʱ��msΪ��λ
	uint8_t   	MaxTryCount; 		//������Դ���
	uint8_t   	MaxResetCount; 		//�����������
	pFunc		ATRxCpltCallback;	//ATָ�������ɣ�ָ���ص�����
} ATCommandConfig;

void AT_Callback(char * str);
void CGSN_Callback(char * str);


static const ATCommandConfig ATCommandList[]=
{
	{AT,	"AT\r\n",	EXEXCMD,500,100,5,3,	AT_Callback		},
	{CGSN,	"AT+CGSN=1",EXEXCMD,500,100,5,3,	CGSN_Callback	},
};

#endif
