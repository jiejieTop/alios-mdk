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
ktask_t *AppTaskCreate_Handle = NULL;
ktask_t *LED_Task_Handle = NULL;



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


/******************************* 全局变量声明 ************************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些全局变量。
 */


/*
*************************************************************************
*                             函数声明
*************************************************************************
*/

static void AppTaskCreate(void* arg);/* 用于创建任务 */
static void LED_Task(void* arg);/* LED_Task任务实现 */


/*****************************************************************
  * @brief  主函数
  * @param  无
  * @retval 无
  * @note   第一步：开发板硬件初始化，alios系统初始化 
            第二步：创建APP应用任务
            第三步：启动alios，开始多任务调度
  ****************************************************************/
int main()
{
  /* 用于初始化板载相关资源 */
  BSP_Init();
  
  /* alios系统初始化 */
  krhino_init();
  
  /* 创建AppTaskCreate任务 */
	
//	aos_task_new_ext();
	
  krhino_task_dyn_create(&AppTaskCreate_Handle, 
                          "AppTaskCreate",
                          NULL, 
                          1,
                          0, 
                          1024 / sizeof(cpu_stack_t),
                          AppTaskCreate,
                          1u);
                          
  /* 启动任务，开启调度 */
  krhino_start();
                          
  while(1);   /* 正常不会执行到这里 */  
}

static void AppTaskCreate(void* arg)
{
  
  /* 创建LED_Task任务 */
  krhino_task_dyn_create(&LED_Task_Handle, 
                          "LED_Task",
                          NULL, 
                          2,
                          0, 
                          1024 / sizeof(cpu_stack_t),
                          LED_Task,
                          1u);
  while(1)
  {
    LED2_ON;
    printf("LED_Task Running,LED2_ON\r\n");
    krhino_task_sleep(1000);   /* 延时500个tick */
    
    LED2_OFF;     
    printf("LED_Task Running,LED2_OFF\r\n");
    krhino_task_sleep(1000);   /* 延时500个tick */
  }
                            
}

static void LED_Task(void* arg)
{	
  while (1)
  {
    LED1_ON;
    printf("LED_Task Running,LED1_ON\r\n");
    krhino_task_sleep(500);   /* 延时500个tick */
    
    LED1_OFF;     
    printf("LED_Task Running,LED1_OFF\r\n");
    krhino_task_sleep(500);   /* 延时500个tick */
  }
  
}


