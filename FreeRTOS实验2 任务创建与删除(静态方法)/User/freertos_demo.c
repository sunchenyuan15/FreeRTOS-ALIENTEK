/**
 ****************************************************************************************************
 * @file        freertos.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       任务创建与删除静态方法
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 MiniSTM32 V4开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "freertos_demo.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
/*FreeRTOS*********************************************************************************************/
#include "FreeRTOS.h"
#include "task.h"

/******************************************************************************************************/
/*FreeRTOS配置*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
 
#define START_TASK_PRIO         1
#define START_TASK_STACK_SIZE   128  //128 * 4 Byte

TaskHandle_t start_task_handler;
StackType_t start_task_stack[START_TASK_STACK_SIZE];
StaticTask_t start_task_tcb;


/* task1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
 
#define TASK1_PRIO         2
#define TASK1_STACK_SIZE   128  //128 * 4 Byte
StackType_t task1_stack[TASK1_STACK_SIZE];
StaticTask_t task1_tcb;
TaskHandle_t task1_handler;


/* task2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
 
#define TASK2_PRIO         3
#define TASK2_STACK_SIZE   128  //128 * 4 Byte
StackType_t task2_stack[TASK2_STACK_SIZE];
StaticTask_t task2_tcb;
TaskHandle_t task2_handler;


/* task3 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
 
#define TASK3_PRIO         4
#define TASK3_STACK_SIZE   128  //128 * 4 Byte
StackType_t task3_stack[TASK3_STACK_SIZE];
StaticTask_t task3_tcb;
TaskHandle_t task3_handler;


//实现LED0每500ms闪烁一次
void task1( void * pvParameters )
{
    while(1)
    {
        printf("task1 ing\r\n");
        LED0_TOGGLE();
        vTaskDelay(500);//阻塞 释放CPU使用权
    }
}


//实现LED1每500ms闪烁一次
void task2( void * pvParameters )
{
    while(1)
    {
        printf("task2 ing\r\n");
        LED1_TOGGLE();
        vTaskDelay(500);//阻塞
    }
}


//判断按键KEY0是否按下，按下则删掉task1
void task3( void * pvParameters )
{
    uint8_t key = 0;
    while(1)
    {
        printf("task3 ing\r\n");
        key = key_scan(0);
        if(key == KEY0_PRES)
        {
            if(task1_handler != NULL)//避免重复删除89p
            {
                printf("deleting task3\r\n");
                vTaskDelete(task1_handler);
                task1_handler = NULL;
            }
            
        }
        vTaskDelay(10);//阻塞
    }
}


void start_task( void * pvParameters )
{
    
    taskENTER_CRITICAL(); //进入临界区：禁用任务调度器（scheduler）和中断

    task1_handler = xTaskCreateStatic( (TaskFunction_t) task1,             //指向任务函数的指针
                                       (char *)         "task1",           //任务函数名
                                       (uint32_t)       TASK1_STACK_SIZE,  //任务堆栈大小
                                       (void *)         NULL,              //传递的任务函数参数
                                       (UBaseType_t)    TASK1_PRIO,        //任务优先级
                                       (StackType_t *)  task1_stack,       //任务堆栈
                                       (StaticTask_t *) &task1_tcb         //任务控制块指针
                                     );
                       
                       
    task2_handler = xTaskCreateStatic( (TaskFunction_t) task2,             //指向任务函数的指针
                                       (char *)         "task2",           //任务函数名
                                       (uint32_t)       TASK2_STACK_SIZE,  //任务堆栈大小
                                       (void *)         NULL,              //传递的任务函数参数
                                       (UBaseType_t)    TASK2_PRIO,        //任务优先级
                                       (StackType_t *)  task2_stack,       //任务堆栈
                                       (StaticTask_t *) &task2_tcb         //任务控制块指针
                                     );
                                       
                                       
    task3_handler = xTaskCreateStatic( (TaskFunction_t) task3,             //指向任务函数的指针
                                       (char *)         "task3",           //任务函数名
                                       (uint32_t)       TASK3_STACK_SIZE,  //任务堆栈大小
                                       (void *)         NULL,              //传递的任务函数参数
                                       (UBaseType_t)    TASK3_PRIO,        //任务优先级
                                       (StackType_t *)  task3_stack,       //任务堆栈
                                       (StaticTask_t *) &task3_tcb         //任务控制块指针
                                      );
                                       
                                       
     vTaskDelete(start_task_handler); //删除任务自身
     taskEXIT_CRITICAL();             //退出临界区
               
}

//空闲任务内存分配
StaticTask_t idle_task_tcb;
StackType_t idle_task_stack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
                                    StackType_t ** ppxIdleTaskStackBuffer,
                                    uint32_t * pulIdleTaskStackSize )
{
    * ppxIdleTaskTCBBuffer = &idle_task_tcb;
    * ppxIdleTaskStackBuffer = idle_task_stack;
    * pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
    
}

//软件定时器内存分配
StaticTask_t timer_task_tcb;
StackType_t timer_task_stack[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer,
                                     StackType_t ** ppxTimerTaskStackBuffer,
                                     uint32_t * pulTimerTaskStackSize )
{
    * ppxTimerTaskTCBBuffer = &timer_task_tcb;
    * ppxTimerTaskStackBuffer = timer_task_stack;
    * pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
    
}

/******************************************************************************************************/

/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{
    
    start_task_handler = xTaskCreateStatic( (TaskFunction_t) start_task,             //指向任务函数的指针
                                            (char *)         "start_task",           //任务函数名
                                            (uint32_t)       START_TASK_STACK_SIZE,  //任务堆栈大小
                                            (void *)         NULL,                   //传递的任务函数参数
                                            (UBaseType_t)    START_TASK_PRIO,        //任务优先级
                                            (StackType_t *)  start_task_stack,       //任务堆栈
                                            (StaticTask_t *) &start_task_tcb         //任务控制块指针
                                          );
                                        
    vTaskStartScheduler();                                      //任务调度
}

