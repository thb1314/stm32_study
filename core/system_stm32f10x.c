#include "stm32f10x.h"


/*!< Uncomment the following line if you need to relocate your vector Table in
     Internal SRAM. */ 
// #define VECT_TAB_SRAM
#define VECT_TAB_OFFSET  0x0 


// 定义时钟来源 HSE或者HSI 根据是否需要倍频选择使用PLL
#define SYSCLK_FREQ_SOURCE_HSE


#if defined (STM32F10X_LD_VL) || (defined STM32F10X_MD_VL) || (defined STM32F10X_HD_VL)
/* #define SYSCLK_FREQ_HSE    HSE_VALUE */
	#define SYSCLK_FREQ_24MHz  24000000
#else


// #define SYSCLK_FREQ_HSE    HSE_VALUE
// #define SYSCLK_FREQ_24MHz  24000000
/* #define SYSCLK_FREQ_36MHz  36000000 */
/* #define SYSCLK_FREQ_48MHz  48000000 */
/* #define SYSCLK_FREQ_56MHz  56000000 */
// #define SYSCLK_FREQ_64MHz  64000000
#define SYSCLK_FREQ_72MHz  72000000
#endif


/*
小容量是指闪存存储器容量在16K至32K字节之间。
中容量是指闪存存储器容量在64K至128K字节之间。
大容量是指闪存存储器容量在256K至512K字节之间。
STM32F10X_LD_VL 对应于 LowDensity ValueLine，即小容量的STM32F100
STM32F10X_MD_VL 对应于 MediumDensity ValueLine，即中等容量的STM32F100
STM32F10X_HD_VL 对应于 HighDensity ValueLine，即大容量的STM32F100
STM32F10X_CL 对应于 ConnectivityLine，即STM32F105、STM32F107
STM32F10X_LD 对应于 LowDensity ，即小容量的STM32F103、101
STM32F10X_MD 对应于 MediumDensity ，即中等容量的STM32F103、101
STM32F10X_HD 对应于 HighDensity ，即大容量的STM32F103、101
*/

/*******************************************************************************
*  Clock Definitions
*******************************************************************************/
#ifdef SYSCLK_FREQ_24MHz
  uint32_t SystemCoreClock         = SYSCLK_FREQ_24MHz;        /*!< System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_36MHz
  uint32_t SystemCoreClock         = SYSCLK_FREQ_36MHz;        /*!< System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_48MHz
  uint32_t SystemCoreClock         = SYSCLK_FREQ_48MHz;        /*!< System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_56MHz
  uint32_t SystemCoreClock         = SYSCLK_FREQ_56MHz;        /*!< System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_64MHz
  uint32_t SystemCoreClock         = SYSCLK_FREQ_64MHz;        /*!< System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_72MHz
  uint32_t SystemCoreClock         = SYSCLK_FREQ_72MHz;        /*!< System Clock Frequency (Core Clock) */
#else
  uint32_t SystemCoreClock         = HSI_VALUE;        /*!< System Clock Frequency (Core Clock) */
#endif


static void SetSysClock(void);



#ifdef DATA_IN_ExtSRAM
  static void SystemInit_ExtMemCtl(void); 
#endif /* DATA_IN_ExtSRAM */



void SystemInit(void)
{
	/*
	HSE 高速外部时钟信号
	HSI 高速内部时钟信号
	LSI 低速内部时钟信号
	LSE 低速外部时钟信号
	*/
	//内部时钟打开
	B_RCC->CR.HSION = 1;

	//原来STM32F10X_CL代表的是使用了 STM32互联系列微处理器也就是STM32F105和STM32F107系列处理器。
	/* Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits */
	#ifndef STM32F10X_CL
		RCC->CFGR &= (uint32_t)0xF8FF0000;
	#else
		RCC->CFGR &= (uint32_t)0xF0FF0000;
	#endif /* STM32F10X_CL */   
	
	/* Reset HSEON, CSSON and PLLON bits */
    //RCC->CR &= (uint32_t)0xFEF6FFFF;
	B_RCC->CR.HSEON = 0;
	B_RCC->CR.CSSON = 0;
	B_RCC->CR.PLLON = 0;
	/* Reset HSEBYP bit */
    //RCC->CR &= (uint32_t)0xFFFBFFFF;
	// 外部高速时钟旁路 (External high-speed clock bypass)
	// 0：外部3-25MHz振荡器没有旁路
	B_RCC->CR.HSEBYP = 0;
	
	
	/* Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits */
	RCC->CFGR &= (uint32_t)0xFF80FFFF;
	
	
	#ifdef STM32F10X_CL
	  /* Reset PLL2ON and PLL3ON bits */
	  RCC->CR &= (uint32_t)0xEBFFFFFF;
	  /* Disable all interrupts and clear pending bits  */
	  RCC->CIR = 0x00FF0000;
	  /* Reset CFGR2 register */
	  RCC->CFGR2 = 0x00000000;
	#elif defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || (defined STM32F10X_HD_VL)
	  /* Disable all interrupts and clear pending bits  */
	  RCC->CIR = 0x009F0000;

	  /* Reset CFGR2 register */
	  RCC->CFGR2 = 0x00000000;      
	#else
	  /* Disable all interrupts and clear pending bits  */
	  RCC->CIR = 0x009F0000;
	#endif /* STM32F10X_CL */
	#if defined (STM32F10X_HD) || (defined STM32F10X_XL) || (defined STM32F10X_HD_VL)
	  // 如果定义使用外部RAM
	  #ifdef DATA_IN_ExtSRAM
		SystemInit_ExtMemCtl(); 
	  #endif /* DATA_IN_ExtSRAM */
	#endif 
	
	SetSysClock();
	
	#ifdef VECT_TAB_SRAM
	  SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM. */
	#else
	  SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH. */
	#endif 
}



static void SetSysClock(void)
{
	__IO uint32_t PLL = 0;
	#ifdef SYSCLK_FREQ_SOURCE_HSE
	//外部时钟使能
	B_RCC->CR.HSEON = 1;
	//等待外部时钟稳定
	while(0 == B_RCC->CR.HSERDY);
	#else
	B_RCC->CR.HSION = 1;
	//等待内部时钟稳定
	while(0 == B_RCC->CR.HSIRDY);
	#endif

#if !defined STM32F10X_LD_VL && !defined STM32F10X_MD_VL && !defined STM32F10X_HD_VL 
   
	/* Enable Prefetch Buffer 启用预存缓冲区 */
    FLASH->ACR |= FLASH_ACR_PRFTBE;
	
#endif
	
	//设置FLASH访问时延
	/*
	000：零等待状态，当 0 < SYSCLK ≤ 24MHz
	001：一个等待状态，当 24MHz < SYSCLK ≤ 48MHz
	010：两个等待状态，当 48MHz < SYSCLK ≤ 72MHz
	*/
	if(SystemCoreClock <= 24000000)
	{
		B_FLASH->ACR.LATENCY[2] = 0;
		B_FLASH->ACR.LATENCY[1] = 0;
		B_FLASH->ACR.LATENCY[0] = 0;
	}
	else if(SystemCoreClock > 24000000 && SystemCoreClock <= 48000000)
	{
		B_FLASH->ACR.LATENCY[2] = 0;
		B_FLASH->ACR.LATENCY[1] = 0;
		B_FLASH->ACR.LATENCY[0] = 1;
	}
	else
	{
		B_FLASH->ACR.LATENCY[2] = 0;
		B_FLASH->ACR.LATENCY[1] = 1;
		B_FLASH->ACR.LATENCY[0] = 0;
	}
	 
	// APB1 = DIV2 APB2 = DIV1 AHB = DIV1
	
	// HPRE[3:0]： AHB预分频 (AHB Prescaler)
	// 0xxx： SYSCLK不分频
	B_RCC->CFGR.HPRE[3] = 0;

	// 低速APB预分频(APB1) (APB low-speed prescaler (APB1))
	
	B_RCC->CFGR.PPRE1[2] = 1;
	B_RCC->CFGR.PPRE1[1] = 0;
	B_RCC->CFGR.PPRE1[0] = 0;
	// 高速APB预分频(APB2) (APB high-speed prescaler (APB2))
	// 0xx： HCLK不分频
	B_RCC->CFGR.PPRE2[2] = 0;
	
	//计算倍频
	#ifdef SYSCLK_FREQ_SOURCE_HSE
		PLL = SystemCoreClock / HSE_VALUE;
	#else
		PLL = SystemCoreClock / (HSI_VALUE / 2);
	#endif
	//设置倍频 至少是二倍频 如果是不倍频则说明是HSE直接作为SYSCLK
	if(PLL >= 2)
	{
		PLL -= 2;
		if(PLL > 15)
			PLL = 15;
		//设置PLL值 2~16
		RCC->CFGR |= PLL<<18;  
		
		
		
		
		#ifdef SYSCLK_FREQ_SOURCE_HSE
		// 设置PLLXTPRE HSE分频器 HSE不分频
		B_RCC->CFGR.PLLXTPRE = 0;
		// PLLSRC 选择HSE
		B_RCC->CFGR.PLLSRC = 1;
		#else
		// PLLSRC 选择HSI/2
		B_RCC->CFGR.PLLSRC = 0;
		#endif
		
		
		//打开PLL
		B_RCC->CR.PLLON = 1;
		//等待PLL锁定
		while(0 == B_RCC->CR.PLLRDY);
		//PLL作为系统时钟
		// SW
		/*
		00： HSI作为系统时钟；
		01： HSE作为系统时钟；
		10： PLL输出作为系统时钟；
		11：不可用。
		*/
		B_RCC->CFGR.SW[1] = 1;
		B_RCC->CFGR.SW[0] = 0;
		while(!((1 == B_RCC->CFGR.SWS[1]) && (0 == B_RCC->CFGR.SWS[0])));
	}
	else
	{
		// 不倍频 HSE直接作为系统时钟
		// SW
		/*
		00： HSI作为系统时钟；
		01： HSE作为系统时钟；
		10： PLL输出作为系统时钟；
		11：不可用。
		*/
		#ifdef SYSCLK_FREQ_SOURCE_HSE
		B_RCC->CFGR.SW[1] = 0;
		B_RCC->CFGR.SW[0] = 1;
		while(!((0 == B_RCC->CFGR.SWS[1]) && (1 == B_RCC->CFGR.SWS[0])));
		#else
		B_RCC->CFGR.SW[1] = 0;
		B_RCC->CFGR.SW[0] = 0;
		while(!((0 == B_RCC->CFGR.SWS[1]) && (0 == B_RCC->CFGR.SWS[0])));
		#endif
		
	}
		

}




/**
  * @brief  Setup the external memory controller. Called in startup_stm32f10x.s 
  *          before jump to __main
  * @param  None
  * @retval None
  */ 
#ifdef DATA_IN_ExtSRAM
/**
  * @brief  Setup the external memory controller. 
  *         Called in startup_stm32f10x_xx.s/.c before jump to main.
  * 	      This function configures the external SRAM mounted on STM3210E-EVAL
  *         board (STM32 High density devices). This SRAM will be used as program
  *         data memory (including heap and stack).
  * @param  None
  * @retval None
  */ 
void SystemInit_ExtMemCtl(void) 
{
/*!< FSMC Bank1 NOR/SRAM3 is used for the STM3210E-EVAL, if another Bank is 
  required, then adjust the Register Addresses */

  /* Enable FSMC clock */
  RCC->AHBENR = 0x00000114;
  
  /* Enable GPIOD, GPIOE, GPIOF and GPIOG clocks */  
  RCC->APB2ENR = 0x000001E0;
  
/* ---------------  SRAM Data lines, NOE and NWE configuration ---------------*/
/*----------------  SRAM Address lines configuration -------------------------*/
/*----------------  NOE and NWE configuration --------------------------------*/  
/*----------------  NE3 configuration ----------------------------------------*/
/*----------------  NBL0, NBL1 configuration ---------------------------------*/
  
  GPIOD->CRL = 0x44BB44BB;  
  GPIOD->CRH = 0xBBBBBBBB;

  GPIOE->CRL = 0xB44444BB;  
  GPIOE->CRH = 0xBBBBBBBB;

  GPIOF->CRL = 0x44BBBBBB;  
  GPIOF->CRH = 0xBBBB4444;

  GPIOG->CRL = 0x44BBBBBB;  
  GPIOG->CRH = 0x44444B44;
   
/*----------------  FSMC Configuration ---------------------------------------*/  
/*----------------  Enable FSMC Bank1_SRAM Bank ------------------------------*/
  
  FSMC_Bank1->BTCR[4] = 0x00001011;
  FSMC_Bank1->BTCR[5] = 0x00000200;
}
#endif /* DATA_IN_ExtSRAM */
