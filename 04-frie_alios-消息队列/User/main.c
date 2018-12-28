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
static aos_task_t Receive_Task_Handle;
static aos_task_t Send_Task_Handle;


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
aos_queue_t Test_Queue;


/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */
#define  QUEUE_LEN    30  /* ���еĳ��ȣ����ɰ������ٸ���Ϣ */
#define  QUEUE_SIZE   16   /* ������ÿ����Ϣ��С���ֽڣ� */

/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void Receive_Task(void* arg);/* Receive_Task����ʵ�� */
static void Send_Task(void* arg);/* Send_Task����ʵ�� */
static void AppTaskCreate(void* arg);/* Send_Task����ʵ�� */


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
	void *queue_start;
	size_t cpsr;/* ����һ�������ٽ��Ƕ�׵ı��� */
	
	/* ���ж� */
  cpsr = cpu_intrpt_save();
	
	queue_start = (void*)aos_malloc(QUEUE_LEN * QUEUE_SIZE);
	memset(queue_start,0,(QUEUE_LEN * QUEUE_SIZE));
  /* ������Ϣ���� */
	res = aos_queue_new(&Test_Queue,queue_start,(QUEUE_LEN * QUEUE_SIZE),QUEUE_SIZE);
	
  if(0 == res)
    printf("������Ϣ���гɹ�!\r\n");
	
  /* ����Receive_Task���� */
  res = aos_task_new_ext( &Receive_Task_Handle, /* ������ */ 
													"Receive_Task",/* �������� */
													Receive_Task,/* ������ں��� */
													NULL, /* ������ں������� */
													512 ,/* ����ջ��С */
													7);/* ��������ȼ� */
  
  if(0 == res)
    printf("����Receive_Task����ɹ�!\r\n");
	
	/* ����Send_Task���� */
  res = aos_task_new_ext( &Send_Task_Handle, /* ������ */ 
													"Send_Task",/* �������� */
													Send_Task,/* ������ں��� */
													NULL, /* ������ں������� */
													512 ,/* ����ջ��С */
													6);/* ��������ȼ� */
  if(0 == res)
    printf("����Receive_Task����ɹ�!\r\n");
	
	/* ���ж� */										
	cpu_intrpt_restore(cpsr);
	
	/* �����˳�������ִ�� */
	aos_task_exit(0);		
}


/**********************************************************************
  * @ ������  �� Receive_Task
  * @ ����˵���� Receive_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void Receive_Task(void* arg)
{	
	/* ����һ������ֵ */
	int res = 0;
	void *recv;
	unsigned int size;

  while (1)
  {
		res = aos_queue_recv(&Test_Queue,AOS_WAIT_FOREVER,&recv,&size);
		
		/* �ַ���'\0'��һ���ֽ� */
		if(0 == res)
			printf("������%d�ֽ�����,���յ�����Ϊ%s\n",(unsigned int)(size-1),(char*)&recv);

  }
}

/**********************************************************************
  * @ ������  �� Send_Task
  * @ ����˵���� Send_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void Send_Task(void* arg)
{	
	/* ����һ������ֵ */
	int res = 0;
	
	char send1[] = "12345678";
	char send2[] = "ABCDE";
	
  while (1)
  {
    if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON )
    {
			/* KEY1 ������ */
      printf("����һ����Ϣ\n");

			res = aos_queue_send(&Test_Queue,&send1,sizeof(send1));
			
			if(0 == res)
				printf("���ͳɹ�\n");
    } 
		
    if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON )
    {
			/* KEY2 ������ */
      printf("����һ����Ϣ\n");

			res = aos_queue_send(&Test_Queue,&send2,sizeof(send2));
			
			if(0 == res)
				printf("���ͳɹ�\n");
    }
		
    krhino_task_sleep(20);/* ��ʱ20��tick */
		
  }
}
