/**
 ****************************************************************************************************
 * @file        freertos.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       列表项插入与删除
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


/*定义测试列表 列表项*/
List_t      testlist;       /*定义测试列表*/
ListItem_t  listitem1;      /*定义测试列表项1*/
ListItem_t  listitem2;      /*定义测试列表项2*/
ListItem_t  listitem3;      /*定义测试列表项3*/


//任务1：实现LED0每500ms闪烁一次
void task1( void * pvParameters )
{
    while(1)
    {
        LED0_TOGGLE();
        vTaskDelay(500);//阻塞 释放CPU使用权
    }
}


//任务2：列表项插入和删除
void task2( void * pvParameters )
{
    vListInitialise(&testlist);         /*初始化列表*/
    vListInitialiseItem(&listitem1);    /*初始化列表项1*/
    vListInitialiseItem(&listitem2);    /*初始化列表项2*/
    vListInitialiseItem(&listitem3);    /*初始化列表项3*/

    listitem1.xItemValue = 40;
    listitem2.xItemValue = 60;
    listitem3.xItemValue = 50;


    /* 第二步：打印列表和其他列表项的地址 */
    printf("/**************第二步：打印列表和列表项的地址**************/\r\n");
    printf("项目\t\t\t地址\r\n");
    printf("TestList\t\t0x%p\t\r\n", &testlist);
    printf("TestList->pxIndex\t0x%p\t\r\n", testlist.pxIndex);
    printf("TestList->xListEnd\t0x%p\t\r\n", (&testlist.xListEnd));
    printf("ListItem1\t\t0x%p\t\r\n", &listitem1);
    printf("ListItem2\t\t0x%p\t\r\n", &listitem2);
    printf("ListItem3\t\t0x%p\t\r\n", &listitem3);
    printf("/**************************结束***************************/\r\n");


    /* 第三步：列表项1插入列表 */
    printf("/*****************第三步：列表项1插入列表******************/\r\n");
    vListInsert((List_t*    )&testlist,         /* 列表 */
                (ListItem_t*)&listitem1);       /* 列表项 */
    printf("项目\t\t\t\t地址\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (testlist.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (listitem1.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (testlist.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (listitem1.pxPrevious));
    printf("/**************************结束***************************/\r\n"); 


    /* 第四步：列表项2插入列表 */
    printf("/*****************第四步：列表项2插入列表******************/\r\n");
    vListInsert((List_t*    )&testlist,         /* 列表 */
                (ListItem_t*)&listitem2);       /* 列表项 */
    printf("项目\t\t\t\t地址\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (testlist.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (listitem1.pxNext));
    printf("ListItem2->pxNext\t\t0x%p\r\n", (listitem2.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (testlist.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (listitem1.pxPrevious));
    printf("ListItem2->pxPrevious\t\t0x%p\r\n", (listitem2.pxPrevious));
    printf("/**************************结束***************************/\r\n");


    /* 第五步：列表项3插入列表 */
    printf("/*****************第五步：列表项3插入列表******************/\r\n");
    vListInsert((List_t*    )&testlist,         /* 列表 */
                (ListItem_t*)&listitem3);       /* 列表项 */
    printf("项目\t\t\t\t地址\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (testlist.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (listitem1.pxNext));
    printf("ListItem2->pxNext\t\t0x%p\r\n", (listitem2.pxNext));
    printf("ListItem3->pxNext\t\t0x%p\r\n", (listitem3.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (testlist.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (listitem1.pxPrevious));
    printf("ListItem2->pxPrevious\t\t0x%p\r\n", (listitem2.pxPrevious));
    printf("ListItem3->pxPrevious\t\t0x%p\r\n", (listitem3.pxPrevious));
    printf("/**************************结束***************************/\r\n");

    
    /* 第六步：移除列表项2 */
    printf("/*******************第六步：移除列表项2********************/\r\n");
    uxListRemove((ListItem_t*   )&listitem2);   /* 移除列表项 */
    printf("项目\t\t\t\t地址\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (testlist.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (listitem1.pxNext));
    printf("ListItem3->pxNext\t\t0x%p\r\n", (listitem3.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (testlist.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (listitem1.pxPrevious));
    printf("ListItem3->pxPrevious\t\t0x%p\r\n", (listitem3.pxPrevious));
    printf("/**************************结束***************************/\r\n");

    
    /* 第七步：列表末尾添加列表项2 */
    printf("/****************第七步：列表末尾添加列表项2****************/\r\n");
    vListInsertEnd((List_t*     )&testlist,     /* 列表 */
                   (ListItem_t* )&listitem2);   /* 列表项 */
    printf("项目\t\t\t\t地址\r\n");
    printf("TestList->pxIndex\t\t0x%p\r\n", testlist.pxIndex);
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (testlist.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (listitem1.pxNext));
    printf("ListItem2->pxNext\t\t0x%p\r\n", (listitem2.pxNext));
    printf("ListItem3->pxNext\t\t0x%p\r\n", (listitem3.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (testlist.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (listitem1.pxPrevious));
    printf("ListItem2->pxPrevious\t\t0x%p\r\n", (listitem2.pxPrevious));
    printf("ListItem3->pxPrevious\t\t0x%p\r\n", (listitem3.pxPrevious));
    printf("/************************实验结束***************************/\r\n");


    while(1)
    {
        
        vTaskDelay(1000);//阻塞 释放CPU使用权
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

