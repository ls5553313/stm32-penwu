/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   FreeRTOS V9.0.0  + STM32 �̼�������
  *********************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32 ȫϵ�п����� 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 
 
/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/ 
#include <stdlib.h>
#include <stdio.h>
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"
/* ������Ӳ��bspͷ�ļ� */
#include "bsp_led.h"
#include "bsp_key.h"
#include "delay.h"
#include "lcd.h"
#include "gui.h"
#include "./IrDa/bsp_irda.h" 
#include "./usart/bsp_usart.h"
#include "tasks.h"
#include "bsp_adc.h"
#include "./led/bsp_led.h" 
#include "./i2c/bsp_i2c_ee.h"


extern const unsigned char BMP1[];

extern int delay_time;              //��ʱʱ�䵥λ���� 
extern int sec;											//��ʱʱ�䵥λ��
extern char string[20];							//�ַ���,������ʾLCD
extern uint8_t key;									//����ң�ذ�����ֵ
extern __IO uint16_t ADC_ConvertedValue;  //ADCģ��ת����ѹ
extern float ADC_ConvertedValueLocal;     // �ֲ����������ڱ���ת�������ĵ�ѹֵ 	 

/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */
static TaskHandle_t AppTaskCreate_Handle = NULL;/* ���������� */
static TaskHandle_t LCD_Task_Handle = NULL;/* LED������ */
static TaskHandle_t Main_Task_Handle = NULL;/* KEY������ */
static TaskHandle_t Remote_Task_Handle = NULL;/* KEY������ */
static TaskHandle_t ADC_Task_Handle = NULL;/* KEY������ */
static TimerHandle_t Swtmr1_Handle =NULL;   /* �����ʱ����� */

/********************************** �ں˶����� *********************************/
/*
 * �ź�������Ϣ���У��¼���־�飬�����ʱ����Щ�������ں˵Ķ���Ҫ��ʹ����Щ�ں�
 * ���󣬱����ȴ����������ɹ�֮��᷵��һ����Ӧ�ľ����ʵ���Ͼ���һ��ָ�룬������
 * �ǾͿ���ͨ��������������Щ�ں˶���
 *
 * �ں˶���˵���˾���һ��ȫ�ֵ����ݽṹ��ͨ����Щ���ݽṹ���ǿ���ʵ��������ͨ�ţ�
 * �������¼�ͬ���ȸ��ֹ��ܡ�������Щ���ܵ�ʵ��������ͨ��������Щ�ں˶���ĺ���
 * ����ɵ�
 * 
 */
QueueHandle_t Test_Queue =NULL;

/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */

/******************************* �궨�� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩ�궨�塣
 */
#define  QUEUE_LEN    4   /* ���еĳ��ȣ����ɰ������ٸ���Ϣ */
#define  QUEUE_SIZE   4   /* ������ÿ����Ϣ��С���ֽڣ� */


/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void AppTaskCreate(void);/* ���ڴ������� */

static void LCD_Task(void* pvParameters);/* Test_Task����ʵ�� */
static void Main_Task(void* pvParameters);/* KEY_Task����ʵ�� */
static void Remote_Task(void* pvParameters);/* KEY_Task����ʵ�� */
static void ADC_Task(void* pvParameters);/* KEY_Task����ʵ�� */
static void Swtmr1_Callback(void* parameter);

static void BSP_Init(void);/* ���ڳ�ʼ�����������Դ */

/*****************************************************************
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   ��һ����������Ӳ����ʼ�� 
            �ڶ���������APPӦ������
            ������������FreeRTOS����ʼ���������
  ****************************************************************/
int main(void)
{	
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
	
	SystemInit();           //ϵͳ��ʼ��
	
  /* ������Ӳ����ʼ�� */
  BSP_Init();  
 
  /* ����Test_Queue */
  Test_Queue = xQueueCreate((UBaseType_t ) QUEUE_LEN,/* ��Ϣ���еĳ��� */
                            (UBaseType_t ) QUEUE_SIZE);/* ��Ϣ�Ĵ�С */
  
  
   /* ����AppTaskCreate���� */
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* ������ں��� */
                        (const char*    )"AppTaskCreate",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )1, /* ��������ȼ� */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* ������ƿ�ָ�� */ 
  /* ����������� */           
  if(pdPASS == xReturn)
    vTaskStartScheduler();   /* �������񣬿������� */
  else
			NVIC_SysReset();
    return -1;  
  
  while(1);   /* ��������ִ�е����� */    
}


/***********************************************************************
  * @ ������  �� AppTaskCreate
  * @ ����˵���� Ϊ�˷���������е����񴴽����������������������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
  taskENTER_CRITICAL();           //�����ٽ���
  

  xReturn = xTaskCreate((TaskFunction_t )Main_Task,  /* ������ں��� */
                        (const char*    )"Main_Task",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )4, /* ��������ȼ� */
                        (TaskHandle_t*  )&Main_Task_Handle);/* ������ƿ�ָ�� */ 
												
									
	xReturn = xTaskCreate((TaskFunction_t )LCD_Task,  /* ������ں��� */
                        (const char*    )"LCD_Task",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )3, /* ��������ȼ� */
                        (TaskHandle_t*  )&LCD_Task_Handle);/* ������ƿ�ָ�� */ 
	
	xReturn = xTaskCreate((TaskFunction_t )Remote_Task,  /* ������ں��� */
                        (const char*    )"Remote_Task",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )0, /* ��������ȼ� */
                        (TaskHandle_t*  )&Remote_Task_Handle);/* ������ƿ�ָ�� */ 
												
	xReturn = xTaskCreate((TaskFunction_t )ADC_Task,  /* ������ں��� */
                        (const char*    )"ADC_Task",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )2, /* ��������ȼ� */
                        (TaskHandle_t*  )&ADC_Task_Handle);/* ������ƿ�ָ�� */ 
												
	
	Swtmr1_Handle=xTimerCreate((const char*			)"OneShotTimer",
                             (TickType_t			)1000,/* ��ʱ������ 1s */
                             (UBaseType_t			)pdFALSE,/* ����ģʽ */
                             (void*					  )1,/* Ϊÿ����ʱ������һ��������ΨһID */
                             (TimerCallbackFunction_t)Swtmr1_Callback); 
 
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
  taskEXIT_CRITICAL();            //�˳��ٽ���
}



static void LCD_Task(void* parameter)
{
	vTaskDelay(20);			//��ʱ20��tick
	first_page();				//��ʾ��һҳ,��ҳ
	i2c_reading();			//��ȡ�洢����
  while(1)						//ѭ��
	{
		lcd_task1();			//����LCD�Զ�������tasks����	
		vTaskDelay(20);
		
	}
}

static void Remote_Task(void* parameter) //����ң��
{
   while(1)
	 {
		key=Remote_Scan();	
		vTaskDelay(50);/* ��ʱ50��tick */
		key=0;
	 }
}


static void ADC_Task(void* parameter) //ģ����
{
   while(1)
	 {
		//ADC_ConvertedValueLocal =(float) ADC_ConvertedValue/4096*3.3; 
	
		vTaskDelay(200);/* ��ʱ20��tick */

	 }
}

/**********************************************************************
  * @ ������  �� Main_Task
  * @ ����˵���� Main_Task��
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void Main_Task(void* parameter) //������
{
   while(1)
	 {
		 if( main_task1(0) == 1 )
		 {
			 LED1_ON; // ���
			 xTimerChangePeriod(Swtmr1_Handle,delay_time,0);  //������ʱʱ��
			if(Swtmr1_Handle != NULL)
			{
				xTimerStart(Swtmr1_Handle,0);	//������ʱ��
			}
		 }
		vTaskDelay(200);/* ��ʱ20��tick */
	 }


}


/***********************************************************************
  * @ ������  �� Swtmr2_Callback
  * @ ����˵���� �����ʱ��2 �ص���������ӡ�ص�������Ϣ&��ǰϵͳʱ��
  *              �����ʱ���벻Ҫ��������������Ҳ��Ҫ������ѭ����Ӧ������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
static void Swtmr1_Callback(void* parameter)
{	
	LED1_OFF;
}

/***********************************************************************
  * @ ������  �� BSP_Init
  * @ ����˵���� �弶�����ʼ�������а����ϵĳ�ʼ�����ɷ��������������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  *********************************************************************/
static void BSP_Init(void)
{
	/*
	 * STM32�ж����ȼ�����Ϊ4����4bit��������ʾ��ռ���ȼ�����ΧΪ��0~15
	 * ���ȼ�����ֻ��Ҫ����һ�μ��ɣ��Ժ������������������Ҫ�õ��жϣ�
	 * ��ͳһ��������ȼ����飬ǧ��Ҫ�ٷ��飬�мɡ�
	 */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	
	delay_init(72);	     //��ʱ��ʼ��
	
	/* LED ����� ��ʼ�� */
	LED_GPIO_Config();
  
  /* �������������ʼ��	*/
  Key_GPIO_Config();

	/* ����ң�س�ʼ��	*/
	Remote_Init();
	
	/* ���ڳ�ʼ��,�˳�����	*/
	//USART_Config(); 
	//printf("//////////////////////////////// \r\n");
	
	/* ģ������ʼ��	*/
	ADCx_Init();
	
	/* ATC24C02�洢��ʼ��	*/
	i2c_CfgGpio();

	
	LCD_Init();	   //Һ������ʼ��
	LCD_Clear(BLACK); //����
	POINT_COLOR=WHITE; //��������



}

/********************************END OF FILE****************************/
