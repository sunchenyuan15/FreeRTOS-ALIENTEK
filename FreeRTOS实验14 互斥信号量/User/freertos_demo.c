/**
 ****************************************************************************************************
 * @file        freertos.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       互斥信号量
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
#include "./SYSTEM/delay/delay.h"
#include "./MALLOC/malloc.h"

/*FreeRTOS*********************************************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/******************************************************************************************************/
/*FreeRTOS配置*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
 
#define START_TASK_PRIO         1
#define START_TASK_STACK_SIZE   128  //128 * 4 Byte
TaskHandle_t start_task_handler;


/* task1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
 
#define TASK1_PRIO         2
#define TASK1_STACK_SIZE   128  //128 * 4 Byte
TaskHandle_t task1_handler;


/* task2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
 
#define TASK2_PRIO         3
#define TASK2_STACK_SIZE   128  //128 * 4 Byte
TaskHandle_t task2_handler;


/* task3 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
 
#define TASK3_PRIO         4
#define TASK3_STACK_SIZE   128  //128 * 4 Byte
TaskHandle_t task3_handler;

/*互斥信号量*/
QueueHandle_t mutex_semaphore_handler;

/*任务1:
低优先级任务，同高优先级一样的操作，不同的是低优先级任务占用信号量的时间久一点*/
void task1( void * pvParameters )
{
    while(1)
    {
        printf("low_task获取信号量\r\n");
        xSemaphoreTake(mutex_semaphore_handler, portMAX_DELAY);   /*获取信号量并死等*/

        printf("low_task ing\r\n");
        delay_ms(3000);                                           /*即使正在执行 delay_ms，如果有更高优先级的任务就绪，仍会被抢占*/

        printf("low_task释放信号量\r\n");
        xSemaphoreGive(mutex_semaphore_handler);                  /*释放信号量*/
        vTaskDelay(1000);
    }
}


/*任务2:
中等优先级任务，简单的应用任务*/
void task2( void * pvParameters )
{
    while(1)
    {
        printf("middle_task ing\r\n");
        vTaskDelay(1000);   //1s
    }
}


/*任务3:
高优先级任务，会获取互斥信号量，获取成功以后打印提示信息，处理完后释放信号量*/
void task3( void * pvParameters )
{
    while(1)
    {
        printf("high_task获取信号量\r\n");
        xSemaphoreTake(mutex_semaphore_handler, portMAX_DELAY);   /*获取信号量并死等*/

        printf("high_task ing\r\n");
        delay_ms(1000);

        printf("high_task释放信号量\r\n");
        xSemaphoreGive(mutex_semaphore_handler);         /*释放信号量*/
        vTaskDelay(1000);        
    }
}


/*start_task:
用来创建task1 task2 task3任务*/
void start_task( void * pvParameters )
{
    
    taskENTER_CRITICAL(); //进入临界区：禁用任务调度器（scheduler）和中断
    
    xTaskCreate((TaskFunction_t         )   task1,             //任务函数
               (char *                  )   "task1",           //任务名
               (configSTACK_DEPTH_TYPE  )   TASK1_STACK_SIZE,  //任务栈大小
               (void *                  )   NULL,              //任务参数
               (UBaseType_t             )   TASK1_PRIO,        //任务优先级
               (TaskHandle_t *          )   &task1_handler     //任务句柄
               );


    xTaskCreate((TaskFunction_t         )   task2,             //任务函数
               (char *                  )   "task2",           //任务名
               (configSTACK_DEPTH_TYPE  )   TASK2_STACK_SIZE,  //任务栈大小
               (void *                  )   NULL,              //任务参数
               (UBaseType_t             )   TASK2_PRIO,        //任务优先级
               (TaskHandle_t *          )   &task2_handler     //任务句柄
               );
    

    xTaskCreate((TaskFunction_t         )   task3,             //任务函数
               (char *                  )   "task3",           //任务名
               (configSTACK_DEPTH_TYPE  )   TASK3_STACK_SIZE,  //任务栈大小
               (void *                  )   NULL,              //任务参数
               (UBaseType_t             )   TASK3_PRIO,        //任务优先级
               (TaskHandle_t *          )   &task3_handler     //任务句柄
               );


     vTaskDelete(NULL);     //删除任务自身
     taskEXIT_CRITICAL();   //退出临界区
               
}



/******************************************************************************************************/

/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{
    /*创建互斥信号量->获取互斥信号量->释放互斥信号量*/
    mutex_semaphore_handler = xSemaphoreCreateMutex();              /*主动释放*/
    
    if (mutex_semaphore_handler != NULL)
    {
        printf("互斥信号量创建成功\r\n");
    }
    

    xTaskCreate((TaskFunction_t         )   start_task,             //任务函数
               (char *                  )   "start_task",           //任务名
               (configSTACK_DEPTH_TYPE  )   START_TASK_STACK_SIZE,  //任务栈大小
               (void *                  )   NULL,                   //任务参数
               (UBaseType_t             )   START_TASK_PRIO,        //任务优先级
               (TaskHandle_t *          )   &start_task_handler     //任务句柄
               );
    
    vTaskStartScheduler();//任务调度
}

