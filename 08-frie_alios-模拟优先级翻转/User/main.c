/**
  *******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   fire alios ����
  *******************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32 ȫϵ�п����� 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ********************************************************************
  */ 
/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/ 
#include <k_api.h>
#include <aos/aos.h>

#include "soc_init.h"

/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */
static aos_task_t AppTaskCreate_Handle;
static aos_task_t LowPriority_Task_Handle;
static aos_task_t MidPriority_Task_Handle;
static aos_task_t HighPriority_Task_Handle;

/********************************** �ں˶����� *********************************/
/*
 * �ź�������Ϣ���У��¼���־�飬�����ʱ����Щ�������ں˵Ķ���Ҫ��ʹ����Щ�ں�
 * ���󣬱����ȴ����������ɹ�֮��᷵��һ����Ӧ�ľ����ʵ���Ͼ���һ��ָ�룬������
 * �ǾͿ���ͨ��������������Щ�ں˶���
 *
 * �ں˶���˵���˾���һ��ȫ�ֵ����ݽṹ��ͨ����Щ���ݽṹ���ǿ���ʵ��������ͨ�ţ�
 * �������¼�ͬ���ȸ��ֹ��ܡ�������Щ���ܵ�ʵ��������ͨ��������Щ�ں˶���ĺ���
 * ����ɵ�
 * 
 */
aos_mutex_t Test_Sem;


/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */
#define PARKING_SPACE 10
 
/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void AppTaskCreate(void* arg);/* Send_Task����ʵ�� */
static void LowPriority_Task(void* arg);/* LowPriority_Task����ʵ�� */
static void MidPriority_Task(void* arg);/* MidPriority_Task����ʵ�� */
static void HighPriority_Task(void* arg);/* HighPriority_Task����ʵ�� */



/*****************************************************************
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   ��һ����������Ӳ����ʼ����aliosϵͳ��ʼ�� 
            �ڶ���������APPӦ������
            ������������alios����ʼ���������
  ****************************************************************/
int main(void)
{
	/* ����һ������ֵ */
	int res = 0;
	
  /* ���ڳ�ʼ�����������Դ */
  BSP_Init();
	
  /* aliosϵͳ��ʼ�� */
  aos_init();

  /* ��ʼ��SysTick */
  SysTick_Config( SystemCoreClock / RHINO_CONFIG_TICKS_PER_SECOND );	
	
	/* ����Send_Task���� */
  res = aos_task_new_ext( &AppTaskCreate_Handle, /* ������ */ 
													"AppTaskCreate",/* �������� */
													AppTaskCreate,/* ������ں��� */
													NULL, /* ������ں������� */
													512 ,/* ����ջ��С */
													4);/* ��������ȼ� */
	
	/* ����������� */ 
	if(0 == res)
		aos_start();/* �������񣬿������� */
	else
		return -1;

  while(1);   /* ��������ִ�е����� */  
}

/**********************************************************************
  * @ ������  �� AppTaskCreate
  * @ ����˵���� AppTaskCreate��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
	* @ ˵��  	�� Ϊ�˷�������Ĺ����Ժ����������ڴ�AppTaskCreate�д���
  ********************************************************************/
static void AppTaskCreate(void* arg)
{
	/* ����һ������ֵ */
	int res = 0;
	size_t cpsr;/* ����һ�������ٽ��Ƕ�׵ı��� */
	
	/* ���ж� */
  cpsr = cpu_intrpt_save();
	
  /* ����һ���ź��� */
	aos_sem_new(&Test_Sem,1);

  if(0 == res)
    printf("�����ź����ɹ�!\r\n");
	
  /* ����LowPriority_Task���� */
  res = aos_task_new_ext( &LowPriority_Task_Handle, /* ������ */ 
													"LowPriority_Task",/* �������� */
													LowPriority_Task,/* ������ں��� */
													NULL, /* ������ں������� */
													512 ,/* ����ջ��С */
													7);/* ��������ȼ� */
  
  if(0 == res)
    printf("����LowPriority_Task����ɹ�!\r\n");
	
	/* ����MidPriority_Task���� */
  res = aos_task_new_ext( &MidPriority_Task_Handle, /* ������ */ 
													"MidPriority_Task",/* �������� */
													MidPriority_Task,/* ������ں��� */
													NULL, /* ������ں������� */
													512 ,/* ����ջ��С */
													6);/* ��������ȼ� */
  if(0 == res)
    printf("����MidPriority_Task����ɹ�!\r\n");
	
	/* ����HighPriority_Task���� */
  res = aos_task_new_ext( &HighPriority_Task_Handle, /* ������ */ 
													"HighPriority_Task",/* �������� */
													HighPriority_Task,/* ������ں��� */
													NULL, /* ������ں������� */
													512 ,/* ����ջ��С */
													5);/* ��������ȼ� */
  if(0 == res)
    printf("����HighPriority_Task����ɹ�!\r\n");
	
	/* ���ж� */										
	cpu_intrpt_restore(cpsr);
	
	/* �����˳�������ִ�� */
	aos_task_exit(0);		
}


/**********************************************************************
  * @ ������  �� LowPriority_Task
  * @ ����˵���� LowPriority_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void LowPriority_Task(void* parameter)
{	
  static uint32_t i;
	
	/* ����һ������ֵ */
	int res = 0;
	
  while (1)
  {
    printf("LowPriority_Task ��ȡ�ź���\n");
    //��ȡ�ź���,û��ȡ����һֱ�ȴ�
		res = aos_sem_wait(&Test_Sem,/* �ź������ */
                        AOS_WAIT_FOREVER); /* �ȴ�ʱ�� */
    if( 0 == res )
      printf("LowPriority_Task Runing\n\n");
    
    for(i=0;i<2000000;i++)//ģ������ȼ�����ռ���ź���
		{
			krhino_task_yield();//�����������
		}
    
    printf("LowPriority_Task �ͷ��ź���!\r\n");
		
    aos_sem_signal(&Test_Sem);//�ͷ��ź���

		LED1_TOGGLE;
    
    krhino_task_sleep(500);
  }
}

/**********************************************************************
  * @ ������  �� MidPriority_Task
  * @ ����˵���� MidPriority_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void MidPriority_Task(void* parameter)
{	 
  while (1)
  {
   printf("MidPriority_Task Runing\n");
		LED3_TOGGLE;
   krhino_task_sleep(500);
  }
}

/**********************************************************************
  * @ ������  �� HighPriority_Task
  * @ ����˵���� HighPriority_Task ��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void HighPriority_Task(void* parameter)
{	
	/* ����һ������ֵ */
	int res = 0;
  while (1)
  {
    printf("HighPriority_Task ��ȡ�ź���\n");
    //��ȡ�ź���,û��ȡ����һֱ�ȴ�
		res = aos_sem_wait(&Test_Sem,/* �ź������ */
                       AOS_WAIT_FOREVER); /* �ȴ�ʱ�� */
    if( 0 == res )
      printf("HighPriority_Task Runing\n");
		
		LED2_TOGGLE;
		
    aos_sem_signal(&Test_Sem);//�ͷ��ź���

    krhino_task_sleep(500);
  }
}


