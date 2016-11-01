#include "stm32f10x.h"

#define NVIC_PriorityGroup_0         ((uint32_t)0x07) /*!< 0 bits for pre-emption priority
                                                            4 bits for subpriority */
#define NVIC_PriorityGroup_1         ((uint32_t)0x06) /*!< 1 bits for pre-emption priority
                                                            3 bits for subpriority */
#define NVIC_PriorityGroup_2         ((uint32_t)0x05) /*!< 2 bits for pre-emption priority
                                                            2 bits for subpriority */
#define NVIC_PriorityGroup_3         ((uint32_t)0x04) /*!< 3 bits for pre-emption priority
                                                            1 bits for subpriority */
#define NVIC_PriorityGroup_4         ((uint32_t)0x03) /*!< 4 bits for pre-emption priority0  bits for subpriority */
     

#define USART1_REC_MAXLEN 100
uint8_t Usart1_R_Count = 0;
uint8_t Usart1_R_Buff[USART1_REC_MAXLEN] = {0x00};
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
void uart_wirte(uint8_t *s,uint8_t len)
{
	while(len--)
	{
		com_send_char(*s);
		s++;
	}
}
void uart_init(uint32_t baut)
{
	float baurate;	
	uint16_t Mantissa = 0;
	uint16_t Fraction = 0;
	//����1ʱ��
	B_RCC->APB2ENR.USART1EN = 1;
	
	B_RCC->APB2RSTR.USART1RST = 1;
	B_RCC->APB2RSTR.USART1RST = 0;
	// ���ò�����Ϊ115200 Baurate
	baurate = (float)((float)SystemCoreClock / baut / 16.0f);
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
	// ����ʹ��
	B_USART1->CR1.RE = 1;
	
	//�����ж�ʹ��
	NVIC_SetPriority(USART1_IRQn,0x01);
	NVIC_EnableIRQ(USART1_IRQn);
	//�򿪿����ж�
	B_USART1->CR1.IDLEIE = 1;
	//�򿪽����ն�
	B_USART1->CR1.RXNEIE = 1;
}
void USART1_IRQHandler(void)
{
	uint8_t sbuf = 0;
	if(1 == B_USART1->CR1.RXNEIE)
	{
		if(1 == B_USART1->SR.RXNE)
		{
			sbuf = (uint8_t) USART1->DR;
			B_USART1->SR.RXNE = 0;
			if(USART1_REC_MAXLEN < Usart1_R_Count)
				Usart1_R_Count = 0;
			Usart1_R_Buff[Usart1_R_Count++] = sbuf;
		}
	}
	if(1 == B_USART1->CR1.IDLEIE)
	{
		if(1 == B_USART1->SR.IDLE)
		{
			sbuf = (uint8_t) USART1->DR;
			PE->ODR.BIT[6] = !PE->ODR.BIT[6];
			uart_wirte(&Usart1_R_Count,1);
			uart_wirte(Usart1_R_Buff,Usart1_R_Count);
			
			Usart1_R_Count = 0;
		}
		
	}
}
void NVIC_Init(uint32_t NVIC_PreemptionPriority,IRQn_Type IRQn,uint8_t Priority)	 
{ 
	NVIC_SetPriorityGrouping(NVIC_PreemptionPriority);
	NVIC_SetPriority(IRQn,(uint32_t)Priority);
	NVIC_EnableIRQ(IRQn); 				   
}




void EXIT_Init(void)
{
	NVIC_Init(NVIC_PriorityGroup_4,EXTI4_IRQn,0x00);
	if(0 == B_RCC->APB2ENR.IOPEEN)
		B_RCC->APB2ENR.IOPEEN = 1;
	//����PE4Ϊ�������� PE4λ����
	PE->CP.CONFIG[4].CNF[1] = 1;
	PE->CP.CONFIG[4].CNF[0] = 0;
	PE->CP.CONFIG[4].MODE[1] = 0;
	PE->CP.CONFIG[4].MODE[0] = 0;
	PE->ODR.BIT[4] = 1;
	
	
	//ʹ��AFIO �˿ڸ���
	B_RCC->APB2ENR.AFIOEN = 1;
	
	B_AFIO->EXTICR[4/4].EXTI_N[4%4][3] = 0;
	B_AFIO->EXTICR[4/4].EXTI_N[4%4][2] = 1;
	B_AFIO->EXTICR[4/4].EXTI_N[4%4][1] = 0;
	B_AFIO->EXTICR[4/4].EXTI_N[4%4][0] = 0;
	
	//ȡ���ж�����
	B_EXTI->IMR.MR[4] = 1;
	// ��ֹ������
	B_EXTI->RTSR.TR[4] = 0;
	// �����½��ش����ж�
	B_EXTI->FTSR.TR[4] = 1;
}


void TIM_Init(void)	 
{
	//TIM�жϷ���
	NVIC_Init(NVIC_PriorityGroup_4,TIM1_UP_IRQn,0x02);
	B_RCC->APB2ENR.TIM1EN = 1;
	
	B_RCC->APB2RSTR.TIM1RST = 1;
	B_RCC->APB2RSTR.TIM1RST = 0;
	//��װ��ֵ ARR 
	//B_TIM1->CR1.ARPE = 1;
	TIM1->ARR = 7200 - 1;
	//Ԥ��Ƶ�� 7199
	TIM1->PSC = 7200- 1;
	//���������¼�
	//B_TIM1->EGR.UG = 1;
	
	//������   DIR                                                                                                      1.DIR = 0;
	B_TIM1->CR1.DIR = 0;
	//����ж�
	B_TIM1->SR.UIF = 0;
	//��������ж�
	B_TIM1->DIER.UIE = 1;

	//ʹ�ܴ��� 1 
	B_TIM1->CR1.CEN = 1;
	
	//���������¼�
	B_TIM1->EGR.UG = 1;
	
} 
void EXTI4_IRQHandler(void)
{
	B_EXTI->PR.PR[4] = 1;
	PE->ODR.BIT[6] = ~PE->ODR.BIT[6];
}

void TIM1_UP_IRQHandler(void)
{
	
	if(0 != B_TIM1->SR.UIF)
	{
		PE->ODR.BIT[5] = !PE->ODR.BIT[5];
		//����ж�
		B_TIM1->SR.UIF = 0;
	}

	
}
int main(void)
{
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
	PE->ODR.BIT[5] = 1;
	

	uart_init(115200);
	EXIT_Init();
	TIM_Init();
	
	
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
	
	
	while(1);


	
	//PA->ODR.BIT1 = 1;

	//return 0;
}
