#include "log.h"
#include "string.h"


//�ض���fputc
int fputc(int ch, FILE *f)
{
	/* ����һ���ֽ����ݵ�����DEBUG_USART */
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 1000);	
	return (ch);
}

void strrpc(char *str,char *oldstr,char *newstr){
    char bstr[strlen(str)];//ת��������
    memset(bstr,0,sizeof(bstr));
 
    for(int i = 0;i < strlen(str);i++){
        if(!strncmp(str+i,oldstr,strlen(oldstr))){//����Ŀ���ַ���
            strcat(bstr,newstr);
            i += strlen(oldstr) - 1;
        }else{
        	strncat(bstr,str + i,1);//����һ�ֽڽ�������
	    }
    }
    strcpy(str,bstr);
}

void StrNoLR(char * str)
{
	strrpc(str,"\r\n","**");
}
