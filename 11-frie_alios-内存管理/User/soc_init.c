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

/* ϵͳ�ڴ������ */
extern void aos_heap_set(void);

void BSP_Init(void)
{
  aos_heap_set();
  /*
   * STM32�ж����ȼ�����Ϊ4����4bit��������ʾ��ռ���ȼ�����ΧΪ��0~15
   * ���ȼ�����ֻ��Ҫ����һ�μ��ɣ��Ժ������������������Ҫ�õ��жϣ�
   * ��ͳһ��������ȼ����飬ǧ��Ҫ�ٷ��飬�мɡ�
   */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
  
  /* �弶���������Դ�ڴ˳�ʼ�� */
  
	/* LED ��ʼ�� */
	LED_GPIO_Config();

	/* ���ڳ�ʼ��	*/
	USART_Config();
	
  /* ������ʼ��	*/
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
  /* ����һ���ֽ����ݵ����� */
  USART_SendData(DEBUG_USARTx, (uint8_t) ch);
  
  /* �ȴ�������� */
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
  /* �ȴ������������� */
  while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET);

  return (int)USART_ReceiveData(DEBUG_USARTx);
}


