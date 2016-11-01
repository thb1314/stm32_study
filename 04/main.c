#include "stm32f10x.h"

void delay(uint32_t time)
{
	while(time--);
}

int main(void)
{
	//实现按键操作LED
	//使能时钟
	B_RCC->APB2ENR.IOPEEN = 1;
	//设置PE4为上拉输入 PE4位按键
	PE->CP.CONFIG[4].CNF[1] = 1;
	PE->CP.CONFIG[4].CNF[0] = 0;
	PE->CP.CONFIG[4].MODE[1] = 0;
	PE->CP.CONFIG[4].MODE[0] = 0;
	PE->ODR.BIT[4] = 1;
	
	//PE6 设置成推挽输出
	PE->CP.CONFIG[6].CNF[1] = 0;
	PE->CP.CONFIG[6].CNF[0] = 0;
	PE->CP.CONFIG[6].MODE[1] = 1;
	PE->CP.CONFIG[6].MODE[0] = 1;
	PE->ODR.BIT[6] = 1;


	
	//PE6 设置成推挽输出
	PE->CP.CONFIG[5].CNF[1] = 0;
	PE->CP.CONFIG[5].CNF[0] = 0;
	PE->CP.CONFIG[5].MODE[1] = 1;
	PE->CP.CONFIG[5].MODE[0] = 1;
	PE->ODR.BIT[5] = 0;
	
	
	while(1)
	{

		PE->ODR.BIT[6] = ~PE->ODR.BIT[6];
		PE->ODR.BIT[5] = ~PE->ODR.BIT[5];
		delay(1000000);
		
	}


	
	//PA->ODR.BIT1 = 1;

	//return 0;
}
