#include "Hardware.h"

char UartRXBuff[_UART_RXBUFFSIZE];

void UartInit(void)
{
	__HAL_UART_ENABLE_IT(&_ATUART_HANDLE, UART_IT_IDLE);  					//ʹ�ܴ��ڿ����ж�
	HAL_UART_Receive_DMA(&_ATUART_HANDLE, (uint8_t*)UartRXBuff, _UART_RXBUFFSIZE);     	//����DMA����
	__HAL_DMA_DISABLE_IT(&_UART_DMA_HANDLE,DMA_IT_TC);				//�ر�DMA�ж�
	
	__LOG("size: %d",sizeof(ATCommandConfig));
	
}

void SendString(char * str)
{
	HAL_UART_Transmit(& _ATUART_HANDLE,(uint8_t *)str,strlen(str),_UART_TIMEOUT);
}

void USER_UART_IDLECallback(UART_HandleTypeDef *huart)
{
	 uint8_t RXDataLength;
	if(RESET == __HAL_UART_GET_FLAG(& _ATUART_HANDLE, UART_FLAG_IDLE))   
	{	 // �ж��Ƿ��ǿ����ж�
		return;			 
	} 

	// ��������жϱ�־�������һֱ���Ͻ����жϣ�
	__HAL_UART_CLEAR_IDLEFLAG(& _ATUART_HANDLE); 
	// ֹͣ����DMA����
    HAL_UART_DMAStop(& _ATUART_HANDLE);  
                                                       
    // ������յ������ݳ���
    RXDataLength  = _UART_RXBUFFSIZE - __HAL_DMA_GET_COUNTER(&_UART_DMA_HANDLE);   
    
	UartRXBuff[RXDataLength]=0;
	
	// ���Ժ����������յ������ݴ�ӡ��ȥ
	//__LOGARRAY(UartRXBuff,RXDataLength+1,"�жϽ��յ�����");
	 
    __LOGNOLF("Receive Data(length = %d):%s ",RXDataLength,UartRXBuff);                    
	
	//�����ź������ڽ�����һ֡���ݺ󷢲��ź���
	BaseType_t xHightPriorityTaskWoken=pdFALSE;
	xSemaphoreGiveFromISR(ATRXCplSemaphore,&xHightPriorityTaskWoken);
    //�˳��жϺ�ִ��������ȼ�����
	portYIELD_FROM_ISR(xHightPriorityTaskWoken);
  
	// ������ʼDMA���� ÿ��255�ֽ�����
    HAL_UART_Receive_DMA(&_ATUART_HANDLE, (uint8_t*)UartRXBuff, _UART_RXBUFFSIZE);  

}

void MoudleResst(void)
{
	
}

