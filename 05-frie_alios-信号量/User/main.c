/**
  *******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   fire alios 例程
  *******************************************************************
  * @attention
  *
  * 实验平台:野火 STM32 全系列开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ********************************************************************
  */ 
/*
*************************************************************************
*                             包含的头文件
*************************************************************************
*/ 
#include <k_api.h>
#include <aos/aos.h>

#include "soc_init.h"

/**************************** 任务句柄 ********************************/
/* 
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
 */
static aos_task_t AppTaskCreate_Handle;
static aos_task_t Receive_Task_Handle;
static aos_task_t Send_Task_Handle;


/********************************** 内核对象句柄 *********************************/
/*
 * 信号量，消息队列，事件标志组，软件定时器这些都属于内核的对象，要想使用这些内核
 * 对象，必须先创建，创建成功之后会返回一个相应的句柄。实际上就是一个指针，后续我
 * 们就可以通过这个句柄操作这些内核对象。
 *
 * 内核对象说白了就是一种全局的数据结构，通过这些数据结构我们可以实现任务间的通信，
 * 任务间的事件同步等各种功能。至于这些功能的实现我们是通过调用这些内核对象的函数
 * 来完成的
 * 
 */
aos_sem_t Test_Sem;


/******************************* 全局变量声明 ************************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些全局变量。
 */
 
 
/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void Receive_Task(void* arg);/* Receive_Task任务实现 */
static void Send_Task(void* arg);/* Send_Task任务实现 */
static void AppTaskCreate(void* arg);/* Send_Task任务实现 */


/*****************************************************************
  * @brief  主函数
  * @param  无
  * @retval 无
  * @note   第一步：开发板硬件初始化，alios系统初始化 
            第二步：创建APP应用任务
            第三步：启动alios，开始多任务调度
  ****************************************************************/
int main(void)
{
	/* 定义一个返回值 */
	int res = 0;
	
  /* 用于初始化板载相关资源 */
  BSP_Init();
	
  /* alios系统初始化 */
  aos_init();

  /* 初始化SysTick */
  SysTick_Config( SystemCoreClock / RHINO_CONFIG_TICKS_PER_SECOND );	
	
	/* 创建Send_Task任务 */
  res = aos_task_new_ext( &AppTaskCreate_Handle, /* 任务句柄 */ 
													"AppTaskCreate",/* 任务名字 */
													AppTaskCreate,/* 任务入口函数 */
													NULL, /* 任务入口函数参数 */
													512 ,/* 任务栈大小 */
													4);/* 任务的优先级 */
	
	/* 启动任务调度 */ 
	if(0 == res)
		aos_start();/* 启动任务，开启调度 */
	else
		return -1;

  while(1);   /* 正常不会执行到这里 */  
}

/**********************************************************************
  * @ 函数名  ： AppTaskCreate
  * @ 功能说明： AppTaskCreate任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
	* @ 说明  	： 为了方便任务的管理，以后所有任务在从AppTaskCreate中创建
  ********************************************************************/
static void AppTaskCreate(void* arg)
{
	/* 定义一个返回值 */
	int res = 0;
	size_t cpsr;/* 定义一个保存临界段嵌套的变量 */
	
	/* 关中断 */
  cpsr = cpu_intrpt_save();
	
  /* 创建一个信号量 */
	aos_sem_new(&Test_Sem,0);
	
  if(0 == res)
    printf("创建信号量列成功!\r\n");
	
  /* 创建Receive_Task任务 */
  res = aos_task_new_ext( &Receive_Task_Handle, /* 任务句柄 */ 
													"Receive_Task",/* 任务名字 */
													Receive_Task,/* 任务入口函数 */
													NULL, /* 任务入口函数参数 */
													512 ,/* 任务栈大小 */
													7);/* 任务的优先级 */
  
  if(0 == res)
    printf("创建Receive_Task任务成功!\r\n");
	
	/* 创建Send_Task任务 */
  res = aos_task_new_ext( &Send_Task_Handle, /* 任务句柄 */ 
													"Send_Task",/* 任务名字 */
													Send_Task,/* 任务入口函数 */
													NULL, /* 任务入口函数参数 */
													512 ,/* 任务栈大小 */
													6);/* 任务的优先级 */
  if(0 == res)
    printf("创建Receive_Task任务成功!\r\n");
	
	/* 开中断 */										
	cpu_intrpt_restore(cpsr);
	
	/* 任务退出，不再执行 */
	aos_task_exit(0);		
}


/**********************************************************************
  * @ 函数名  ： Receive_Task
  * @ 功能说明： Receive_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void Receive_Task(void* arg)
{	
	/* 定义一个返回值 */
	int res = 0;
	
  while (1)
  {
		//获取二值信号量 xSemaphore,没获取到则一直等待
		res = aos_sem_wait(&Test_Sem,/* 信号量句柄 */
											 AOS_WAIT_FOREVER); /* 等待时间 */
		if(0 == res)
      printf("信号量获取成功!\n\n");
		
		LED1_TOGGLE;
  }
}

/**********************************************************************
  * @ 函数名  ： Send_Task
  * @ 功能说明： Send_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void Send_Task(void* arg)
{	
  while (1)
  {
    if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON )
    {
			/* KEY1 被按下 */
      printf("释放一个信号量\n");
			aos_sem_signal(&Test_Sem);
    } 
		
    if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON )
    {
			/* KEY2 被按下 */
      printf("释放一个信号量\n");
			aos_sem_signal(&Test_Sem);
    }
		
    krhino_task_sleep(20);/* 延时20个tick */
		
  }
}
