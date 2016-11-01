#include "stm32f10x.h"

int main(void)
{
	//GPIO的配置与使用
	/*
	PA->CRL.CNF0[0] =  0;
	PA->CRL.CNF0[1] =  1;
	PA->CRL.MODE0[0] = 1;
	PA->CRL.MODE0[1] = 1;
	
	
	PA->CRL.CNF1[1] =  0;
	PA->CRL.CNF1[0] =  1;
	
	
	PA->LCKR.LCK[0] = 1;
	PA->LCKR.LCK[1] = 1;
	PA->LCKR.LCKR	= 1;
	PA->LCKR.LCKR	= 0;
	PA->LCKR.LCKR	= 1;
	if(0 == PA->LCKR.LCKR)
	{
		if(1 == PA->LCKR.LCKR)
		{
			while(1)
			{
				PA->ODR.BIT[0] = ~PA->IDR.BIT[1];

			}
		}
	}
	*/
	
	//PA->ODR.BIT1 = 1;

	//return 0;
}
