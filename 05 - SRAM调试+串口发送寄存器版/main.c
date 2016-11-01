#include "stm32f10x.h"

void delay(uint32_t time)
{
	while(time--);
}


void com_send_char(uint8_t c)
{

	USART1->DR = (uint16_t)c;
	while(0 == B_USART1->SR.TC);
	B_USART1->SR.TC = 0;
}
void com_send_str(uint8_t *s)
{
	while(*s != '\0')
	{
		com_send_char(*s);
		*s++;
	}
}
int main(void)
{
	float baurate;
	uint16_t Mantissa = 0;
	uint16_t Fraction = 0;
	//����LED
	//ʹ��ʱ��
	B_RCC->APB2ENR.IOPEEN = 1;

	//PE6 ���ó��������
	PE->CP.CONFIG[6].CNF[1] = 0;
	PE->CP.CONFIG[6].CNF[0] = 0;
	PE->CP.CONFIG[6].MODE[1] = 1;
	PE->CP.CONFIG[6].MODE[0] = 1;
	PE->ODR.BIT[6] = 1;


	
	//PE6 ���ó��������
	PE->CP.CONFIG[5].CNF[1] = 0;
	PE->CP.CONFIG[5].CNF[0] = 0;
	PE->CP.CONFIG[5].MODE[1] = 1;
	PE->CP.CONFIG[5].MODE[0] = 1;
	PE->ODR.BIT[5] = 0;
	
	
	//����1ʵ��
	//ʹ��UART1 IO��
	B_RCC->APB2ENR.IOPAEN = 1;
	// TX ���츴����� Rx ������������������
	
	//PA9 Tx
	
	PA->CP.CONFIG[9].MODE[1] = 1;
	PA->CP.CONFIG[9].MODE[0] = 1;
	PA->CP.CONFIG[9].CNF[1] = 1;
	PA->CP.CONFIG[9].CNF[0] = 0;
	
	//PA10 Rx
	PA->CP.CONFIG[10].MODE[1] = 0;
	PA->CP.CONFIG[10].MODE[0] = 0;
	PA->CP.CONFIG[10].CNF[1] = 0;
	PA->CP.CONFIG[10].CNF[0] = 1;
	
	//����1ʱ��
	B_RCC->APB2ENR.USART1EN = 1;
	
	B_RCC->APB2RSTR.USART1RST = 1;
	B_RCC->APB2RSTR.USART1RST = 0;
	// ���ò�����Ϊ115200 Baurate
	baurate = (float)((float)SystemCoreClock / 115200.0f / 16.0f);
	Mantissa = (uint16_t) baurate;
	
	baurate -= (uint32_t) baurate;
	
	baurate *= 16;
	
	Fraction = (uint16_t) baurate;
	Fraction &= 0x000F;
	USART1->BRR |=  Fraction;
	
	
	Mantissa <<= 4;
	Mantissa &= 0xFFF0;
	USART1->BRR |= Mantissa;
	
	//ʹ�ܴ���
	B_USART1->CR1.UE = 1;
	//�ֳ� 0 8λ
	B_USART1->CR1.M  = 0;
	//��ʹ��У��
	B_USART1->CR1.PCE = 0;
	// ����ʹ��
	B_USART1->CR1.TE = 1;
	
	
	while(1)
	{

		PE->ODR.BIT[6] = ~PE->ODR.BIT[6];
		PE->ODR.BIT[5] = ~PE->ODR.BIT[5];
		delay(1000000);
		com_send_str("Hello Stm32\n");
		
		
	}


	
	//PA->ODR.BIT1 = 1;

	//return 0;
}
