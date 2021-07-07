/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
 
#include "soc_init.h"


#if defined (__CC_ARM) && defined(__MICROLIB)

#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
  
size_t g_iram1_start = 0x20000000;
size_t g_iram1_total_size = 0x00005000;

#elif defined(__ICCARM__)
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#else
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#endif /* defined (__CC_ARM) && defined(__MICROLIB) */

/* 系统内存初设置 */
extern void aos_heap_set(void);

void BSP_Init(void)
{
  aos_heap_set();
  /*
   * STM32中断优先级分组为4，即4bit都用来表示抢占优先级，范围为：0~15
   * 优先级分组只需要分组一次即可，以后如果有其他的任务需要用到中断，
   * 都统一用这个优先级分组，千万不要再分组，切忌。
   */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
  
  /* 板级相关外设资源在此初始化 */
  
	/* LED 初始化 */
	LED_GPIO_Config();

	/* 串口初始化	*/
	USART_Config();
	
  /* 按键初始化	*/
  Key_GPIO_Config(); 
}


/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  krhino_intrpt_enter();
  krhino_tick_proc();
  krhino_intrpt_exit();
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* 发送一个字节数据到串口 */
  USART_SendData(DEBUG_USARTx, (uint8_t) ch);
  
  /* 等待发送完毕 */
  while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);		

  return (ch);
}

/**
  * @brief  Retargets the C library scanf function to the USART.
  * @param  None
  * @retval None
  */
GETCHAR_PROTOTYPE
{
  /* 等待串口输入数据 */
  while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET);

  return (int)USART_ReceiveData(DEBUG_USARTx);
}


