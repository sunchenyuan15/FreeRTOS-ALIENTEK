/**
 ****************************************************************************************************
 * @file        freertos.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       队列操作
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


char big_data[100] = "数据数据数据shujushujushuju";
/*创建队列->写队列->读队列*/
QueueHandle_t key_queue;
QueueHandle_t big_data_queue;


/*任务1：
当按键key0或key1按下，将键值拷贝到队列key_queue（入队）
当按键key_up按下，将传输大数据，这里拷贝大数据的地址到队列big_data_queue中*/
void task1( void * pvParameters )
{
    uint8_t key_num;
    BaseType_t state = 0;
    char *buf;
    buf = big_data; /*buf = &big_data[0]*/
    while(1)
    {
        key_num = key_scan(0);
        if (key_num == KEY0_PRES || key_num == KEY1_PRES)
        {   
            state = xQueueGenericSend(key_queue, &key_num, portMAX_DELAY, 0);
            if(state == pdTRUE)
            {
                printf("key_queue队列写入成功\r\n");
            }
            else printf("key_queue队列写入失败\r\n");
        }   

        else if (key_num == WKUP_PRES)
        {
            state = xQueueGenericSend(big_data_queue, &buf, portMAX_DELAY, 0);
            if(state == pdTRUE)
            {
                printf("big_data_queue队列写入成功\r\n");
            }
            else printf("big_data_queue队列写入失败\r\n");            
        }
             
        vTaskDelay(10);
    }
}


/*任务2：
读取队列key_queue中的消息（出队），打印出接收到的键值*/
void task2( void * pvParameters )
{
    uint8_t key;
    BaseType_t state;
    while(1)
    {
        state = xQueueReceive( key_queue, &key, portMAX_DELAY );
        if(state == pdTRUE)
        {
            printf("key_queue读取成功，数据：%d\r\n", key);
        }
        else printf("key_queue读取失败");
        vTaskDelay(10);
    }
}


/*任务3：
从队列big_data_queue读取大数据地址，通过地址访问大数据*/
void task3( void * pvParameters )
{
    char * buf; 
    BaseType_t state;
    while(1)
    {
        state = xQueueReceive( big_data_queue, &buf, portMAX_DELAY );
        if(state == pdTRUE)
        {
            printf("读取成功，数据：%s\r\n", buf);
        }
        else printf("big_data_queue读取失败");
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
    
    key_queue = xQueueGenericCreate( 2, sizeof(uint8_t), 0U );
    if(key_queue != NULL)
    {
        printf("key_queue队列创建成功\r\n");
    }
    else printf("key_queue队列创建失败\r\n");

    big_data_queue = xQueueGenericCreate( 1, sizeof(char *), 0u);
    if(big_data_queue != NULL)
    {
        printf("big_data_queue队列创建成功\r\n");
    }
    else printf("big_data_queue队列创建失败\r\n");    


    xTaskCreate((TaskFunction_t         )   start_task,             //任务函数
               (char *                  )   "start_task",           //任务名
               (configSTACK_DEPTH_TYPE  )   START_TASK_STACK_SIZE,  //任务栈大小
               (void *                  )   NULL,                   //任务参数
               (UBaseType_t             )   START_TASK_PRIO,        //任务优先级
               (TaskHandle_t *          )   &start_task_handler     //任务句柄
               );
    
    vTaskStartScheduler();//任务调度
}

