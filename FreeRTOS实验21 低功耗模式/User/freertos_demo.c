/**
 ****************************************************************************************************
 * @file        freertos.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       二值信号量低功耗Tickless模式
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

/*二值信号量*/
QueueHandle_t semaphore_handler;

/*在系统进入低功耗模式前执行的事务*/
void PRE_SLEEP_PROCESSING(void)
{
    __HAL_RCC_GPIOA_CLK_DISABLE();  /*关闭外设时钟*/
    __HAL_RCC_GPIOB_CLK_DISABLE();
    __HAL_RCC_GPIOC_CLK_DISABLE();
    __HAL_RCC_GPIOD_CLK_DISABLE();
}
/*在系统退出低功耗模式后执行的事务*/
void POST_SLEEP_PROCESSING(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();   /*开启外设时钟*/
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
}

/*任务1:
用于按键扫描，当检测到按键KEY0被按下时，释放二值信号量*/
void task1( void * pvParameters )
{
    uint8_t key_num = 0;
    BaseType_t state = 0;
    while(1)
    {
        key_num = key_scan(0);
        if(key_num == KEY0_PRES)
        {
            if(semaphore_handler != NULL)
            {
                state = xSemaphoreGive(semaphore_handler);
                if(state == pdPASS)
                {
                    printf("释放成功\r\n"); //计数值+1
                }
                else if (state == errQUEUE_FULL)
                {
                    printf("释放失败\r\n");
                }                 
            }
        }     

        vTaskDelay(10);
    }
}


/*任务2:
获取二值信号量，当成功获取后打印提示信息*/
void task2( void * pvParameters )
{
    uint8_t i = 0;
    BaseType_t state;
    while(1)
    {
        state = xSemaphoreTake(semaphore_handler, portMAX_DELAY);   /*获取信号量并死等*/
        
        if(state == pdTRUE)
        {
            printf("获取成功\r\n");
        }
        else if(state == pdFALSE)
        {
            printf("已超时%d\r\n", ++i);
        }
    }
}

/*start_task:
用来创建task1和task2任务*/
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
    /*创建二值信号量->释放二值信号量->获取二值信号量*/
    semaphore_handler = xSemaphoreCreateBinary();
    if (semaphore_handler != NULL)
    {
        printf("二值信号量创建成功\r\n");
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

