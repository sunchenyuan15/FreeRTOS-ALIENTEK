/**
 ****************************************************************************************************
 * @file        freertos.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       队列集
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


/*队列集 队列 二值信号量*/
QueueSetHandle_t queueset_handler;
QueueHandle_t queue_handler;
QueueHandle_t semaphore_handler;


/*任务1:
用于扫描按键，当KEY0按下，往队列写入数据，当KEY1按下，释放二值信号量*/
void task1( void * pvParameters )
{
    uint8_t key_num;
    BaseType_t state;
    BaseType_t state1;
    while(1)
    {
        key_num = key_scan(0);
        if(key_num == KEY0_PRES)
        {
            state = xQueueSend(queue_handler, &key_num, portMAX_DELAY);
            if(state == pdTRUE)
            {
                printf("队列写入成功\r\n");
            }
        }
        else if (key_num == KEY1_PRES)
        {
            state1 = xSemaphoreGive(semaphore_handler);
            if(state1 == pdPASS)
            {
                printf("二值信号量释放成功\r\n");
            }
        }    
        vTaskDelay(10);  
    }
}


/*任务2:
读取队列集中的消息，并打印*/
void task2( void * pvParameters )
{
    QueueSetMemberHandle_t handler;
    uint8_t num;
    BaseType_t state;
    while(1)
    {
        handler = xQueueSelectFromSet(queueset_handler, portMAX_DELAY); /*获取队列集中有有效消息的队列*/
        if(handler == queue_handler)            /*队列*/
        {
            xQueueReceive(handler, &num, portMAX_DELAY);
            printf("队列数据：%d\r\n", num);
        }
        else if (handler == semaphore_handler)  /*二值信号量*/
        {
            state = xSemaphoreTake(handler, portMAX_DELAY);
            if (state == pdTRUE)
            {
                printf("获取信号量成功\r\n");
            }
            
        }
        vTaskDelay(10);   
    }
}


/*start_task:
用来创建其它任务，并创建队列集，队列/信号量，将队列/信号量添加到队列集中*/
void start_task( void * pvParameters )
{
    
    taskENTER_CRITICAL(); //进入临界区：禁用任务调度器（scheduler）和中断
    
    /*创建队列集 队列 信号量*/
    queueset_handler = xQueueCreateSet(2);
    if(queueset_handler != NULL)
    {
        printf("队列集创建成功\r\n");
    }

    queue_handler = xQueueCreate(1, sizeof(uint8_t));

    semaphore_handler = xSemaphoreCreateBinary();

    /*添加队列 信号量到队列集*/
    xQueueAddToSet(queue_handler, queueset_handler);
    xQueueAddToSet(semaphore_handler, queueset_handler);


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

