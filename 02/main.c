#include "stm32f10x.h"

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


	while(1)
	{
		if(0 == PE->IDR.BIT[4])
		{
			while(0 == PE->IDR.BIT[4]);
			PE->ODR.BIT[6] = ~PE->ODR.BIT[6];
		}
	}


	
	//PA->ODR.BIT1 = 1;

	//return 0;
}
