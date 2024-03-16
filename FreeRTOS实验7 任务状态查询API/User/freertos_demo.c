/**
 ****************************************************************************************************
 * @file        freertos.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       FreeRTOS实验7 任务状态查询API
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



//任务1：LED0每500ms闪烁一次，提示程序正在运行
void task1( void * pvParameters )
{
    while(1)
    {
        LED0_TOGGLE();
        vTaskDelay(500);
    }
}

char task_buff[400];

//任务2：用于展示任务状态信息查询相关API函数的使用
void task2( void * pvParameters )
{
    UBaseType_t priority_num = 0;
    UBaseType_t task_num = 0;
    UBaseType_t task_num2 = 0;
    TaskStatus_t *status_array = 0;
    TaskStatus_t *status_array_task1 = 0;
    TaskHandle_t task_handle = 0;
    UBaseType_t  task_stack_min = 0;
    eTaskState state = 0;


    vTaskPrioritySet(task2_handler, 8);
    priority_num = uxTaskPriorityGet( task2_handler );
    printf("task2任务优先级 %d\r\n", priority_num); 


    task_num = uxTaskGetNumberOfTasks();
    printf("任务数量 %d\r\n", task_num);


    status_array = mymalloc(SRAMIN, (sizeof(TaskStatus_t) * task_num));
    task_num2 = uxTaskGetSystemState( status_array,
                                      task_num,
                                      NULL );
    printf("任务名\t\t任务编号\t任务优先级\r\n");
    for(uint8_t i=0; i<task_num2; i++)
    {       
        printf("%s\t\t%d\t%d\r\n", 
                status_array[i].pcTaskName, 
                status_array[i].xTaskNumber, 
                status_array[i].uxCurrentPriority);     
    }   


    status_array_task1 = mymalloc(SRAMIN, sizeof(TaskStatus_t));
    vTaskGetInfo( task1_handler,
                  status_array_task1,
                  pdTRUE,
                  eInvalid );
    printf("任务名:%s\r\n", status_array_task1->pcTaskName);
    printf("任务编号:%d\r\n", status_array_task1->xTaskNumber);
    printf("任务优先级:%d\r\n", status_array_task1->uxCurrentPriority);
    printf("任务状态:%d\r\n", status_array_task1->eCurrentState);


    task_handle = xTaskGetHandle( "task1" );
    printf("任务句柄:%#x\r\n", (int)task_handle);//输出0x__
    printf("task1任务句柄:%#x\r\n", (int)task1_handler);


    state = eTaskGetState( task2_handler );
    printf("task2运行状态:%d\r\n", state);


    vTaskList(task_buff);
    printf("系统中任务的信息:\r\n");
    printf("%s\r\n", task_buff);


    while(1)
    {
        // task_stack_min = uxTaskGetStackHighWaterMark( task2_handler );
        // printf("task2历史最小剩余堆栈: %d\r\n", task_stack_min);
        vTaskDelay(1000);
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

