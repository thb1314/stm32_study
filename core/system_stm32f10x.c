#include "stm32f10x.h"


/*!< Uncomment the following line if you need to relocate your vector Table in
     Internal SRAM. */ 
// #define VECT_TAB_SRAM
#define VECT_TAB_OFFSET  0x0 


// ����ʱ����Դ HSE����HSI �����Ƿ���Ҫ��Ƶѡ��ʹ��PLL
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
С������ָ����洢��������16K��32K�ֽ�֮�䡣
��������ָ����洢��������64K��128K�ֽ�֮�䡣
��������ָ����洢��������256K��512K�ֽ�֮�䡣
STM32F10X_LD_VL ��Ӧ�� LowDensity ValueLine����С������STM32F100
STM32F10X_MD_VL ��Ӧ�� MediumDensity ValueLine�����е�������STM32F100
STM32F10X_HD_VL ��Ӧ�� HighDensity ValueLine������������STM32F100
STM32F10X_CL ��Ӧ�� ConnectivityLine����STM32F105��STM32F107
STM32F10X_LD ��Ӧ�� LowDensity ����С������STM32F103��101
STM32F10X_MD ��Ӧ�� MediumDensity �����е�������STM32F103��101
STM32F10X_HD ��Ӧ�� HighDensity ������������STM32F103��101
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
	HSE �����ⲿʱ���ź�
	HSI �����ڲ�ʱ���ź�
	LSI �����ڲ�ʱ���ź�
	LSE �����ⲿʱ���ź�
	*/
	//�ڲ�ʱ�Ӵ�
	B_RCC->CR.HSION = 1;

	//ԭ��STM32F10X_CL�������ʹ���� STM32����ϵ��΢������Ҳ����STM32F105��STM32F107ϵ�д�������
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
	// �ⲿ����ʱ����· (External high-speed clock bypass)
	// 0���ⲿ3-25MHz����û����·
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
	  // �������ʹ���ⲿRAM
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
	//�ⲿʱ��ʹ��
	B_RCC->CR.HSEON = 1;
	//�ȴ��ⲿʱ���ȶ�
	while(0 == B_RCC->CR.HSERDY);
	#else
	B_RCC->CR.HSION = 1;
	//�ȴ��ڲ�ʱ���ȶ�
	while(0 == B_RCC->CR.HSIRDY);
	#endif

#if !defined STM32F10X_LD_VL && !defined STM32F10X_MD_VL && !defined STM32F10X_HD_VL 
   
	/* Enable Prefetch Buffer ����Ԥ�滺���� */
    FLASH->ACR |= FLASH_ACR_PRFTBE;
	
#endif
	
	//����FLASH����ʱ��
	/*
	000����ȴ�״̬���� 0 < SYSCLK �� 24MHz
	001��һ���ȴ�״̬���� 24MHz < SYSCLK �� 48MHz
	010�������ȴ�״̬���� 48MHz < SYSCLK �� 72MHz
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
	
	// HPRE[3:0]�� AHBԤ��Ƶ (AHB Prescaler)
	// 0xxx�� SYSCLK����Ƶ
	B_RCC->CFGR.HPRE[3] = 0;

	// ����APBԤ��Ƶ(APB1) (APB low-speed prescaler (APB1))
	
	B_RCC->CFGR.PPRE1[2] = 1;
	B_RCC->CFGR.PPRE1[1] = 0;
	B_RCC->CFGR.PPRE1[0] = 0;
	// ����APBԤ��Ƶ(APB2) (APB high-speed prescaler (APB2))
	// 0xx�� HCLK����Ƶ
	B_RCC->CFGR.PPRE2[2] = 0;
	
	//���㱶Ƶ
	#ifdef SYSCLK_FREQ_SOURCE_HSE
		PLL = SystemCoreClock / HSE_VALUE;
	#else
		PLL = SystemCoreClock / (HSI_VALUE / 2);
	#endif
	//���ñ�Ƶ �����Ƕ���Ƶ ����ǲ���Ƶ��˵����HSEֱ����ΪSYSCLK
	if(PLL >= 2)
	{
		PLL -= 2;
		if(PLL > 15)
			PLL = 15;
		//����PLLֵ 2~16
		RCC->CFGR |= PLL<<18;  
		
		
		
		
		#ifdef SYSCLK_FREQ_SOURCE_HSE
		// ����PLLXTPRE HSE��Ƶ�� HSE����Ƶ
		B_RCC->CFGR.PLLXTPRE = 0;
		// PLLSRC ѡ��HSE
		B_RCC->CFGR.PLLSRC = 1;
		#else
		// PLLSRC ѡ��HSI/2
		B_RCC->CFGR.PLLSRC = 0;
		#endif
		
		
		//��PLL
		B_RCC->CR.PLLON = 1;
		//�ȴ�PLL����
		while(0 == B_RCC->CR.PLLRDY);
		//PLL��Ϊϵͳʱ��
		// SW
		/*
		00�� HSI��Ϊϵͳʱ�ӣ�
		01�� HSE��Ϊϵͳʱ�ӣ�
		10�� PLL�����Ϊϵͳʱ�ӣ�
		11�������á�
		*/
		B_RCC->CFGR.SW[1] = 1;
		B_RCC->CFGR.SW[0] = 0;
		while(!((1 == B_RCC->CFGR.SWS[1]) && (0 == B_RCC->CFGR.SWS[0])));
	}
	else
	{
		// ����Ƶ HSEֱ����Ϊϵͳʱ��
		// SW
		/*
		00�� HSI��Ϊϵͳʱ�ӣ�
		01�� HSE��Ϊϵͳʱ�ӣ�
		10�� PLL�����Ϊϵͳʱ�ӣ�
		11�������á�
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
