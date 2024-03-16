/**
 ****************************************************************************************************
 * @file        freertos.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       事件标志组
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
#include "event_groups.h"

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


/*创建事件标志组*/
EventGroupHandle_t eventgroup_handler;
#define EVENTBIT_O (1 << 0)
#define EVENTBIT_1 (1 << 1)

/*任务1:
读取按键按下键值，根据不同键值将事件标志组相应事件位置一，模拟事件发生*/
void task1( void * pvParameters )
{
    uint8_t key_num;
    while(1)
    {
        key_num = key_scan(0);
        if(key_num == KEY0_PRES)
        {
            xEventGroupSetBits(eventgroup_handler, EVENTBIT_O); // 将事件标志组bit0位置1
        }
        else if(key_num == KEY1_PRES)
        {
            xEventGroupSetBits(eventgroup_handler, EVENTBIT_1);
        }

        vTaskDelay(10);  
    }
}

 
/*任务2:
同时等待事件标志组中的多个事件位，当这些事件位都置 1 的话就执行相应的处理 */
void task2( void * pvParameters )
{
    EventBits_t event_bit;
    while(1)
    {
        event_bit = xEventGroupWaitBits(eventgroup_handler,         /*事件标志组句柄*/
                                        EVENTBIT_O | EVENTBIT_1,    /*等待bit0 bit1为1*/
                                        pdTRUE,                     /*成功等待事件标志位后清除*/
                                        pdTRUE,                     /*等待bit0 bit1全为1*/
                                        portMAX_DELAY);             /*阻塞时间*/

        printf("等待的事件标志位值是%#x\r\n", event_bit);            /*输出十六进制数，并添加 "0x" 的前缀*/
        vTaskDelay(10);   
    }
}


/*start_task:
用来创建task1和task2任务，并创建事件标志组*/
void start_task( void * pvParameters )
{
    taskENTER_CRITICAL(); //进入临界区：禁用任务调度器（scheduler）和中断
    
    eventgroup_handler = xEventGroupCreate();
    if(eventgroup_handler != NULL)
    {
        printf("事件标志组创建成功\r\n");
    }

    

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
    

    xTaskCreate((TaskFunction_t         )   start_task,             //任务函数
               (char *                  )   "start_task",           //任务名
               (configSTACK_DEPTH_TYPE  )   START_TASK_STACK_SIZE,  //任务栈大小
               (void *                  )   NULL,                   //任务参数
               (UBaseType_t             )   START_TASK_PRIO,        //任务优先级
               (TaskHandle_t *          )   &start_task_handler     //任务句柄
               );
    
    vTaskStartScheduler();//任务调度
}

