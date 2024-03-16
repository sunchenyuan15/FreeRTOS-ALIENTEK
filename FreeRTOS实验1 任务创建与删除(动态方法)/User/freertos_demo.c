/**
 ****************************************************************************************************
 * @file        freertos.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       任务创建与删除动态方法
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
        vTaskDelay(10);
    }
}


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
               
    xTaskCreate((TaskFunction_t         )   task2,
               (char *                  )   "task2",
               (configSTACK_DEPTH_TYPE  )   TASK2_STACK_SIZE,
               (void *                  )   NULL,
               (UBaseType_t             )   TASK2_PRIO,
               (TaskHandle_t *          )   &task2_handler
               );
               
    xTaskCreate((TaskFunction_t         )   task3,
               (char *                  )   "task3",
               (configSTACK_DEPTH_TYPE  )   TASK3_STACK_SIZE,
               (void *                  )   NULL,
               (UBaseType_t             )   TASK3_PRIO,
               (TaskHandle_t *          )   &task3_handler
               );
               
     vTaskDelete(NULL); //删除任务自身
     taskEXIT_CRITICAL(); //退出临界区
               
}


/******************************************************************************************************/

/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{
    xTaskCreate((TaskFunction_t         )   start_task,             //任务函数
               (char *                  )   "start_task",           //任务名
               (configSTACK_DEPTH_TYPE  )   START_TASK_STACK_SIZE,  //任务栈大小
               (void *                  )   NULL,                   //任务参数
               (UBaseType_t             )   START_TASK_PRIO,        //任务优先级
               (TaskHandle_t *          )   &start_task_handler     //任务句柄
               );
               
    vTaskStartScheduler();//任务调度
}

