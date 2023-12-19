#include "devices.h"
#include "mytype.h"
#include "FreeRTOS.h"
#include "task.h"
#include "errno.h"
#include "cmsis_os.h"
#include <string.h>
#include <stdio.h>

osThreadId_t oledHandle;
const osThreadAttr_t oled_attributes = {
	  .name = "oled",
	  .stack_size = 128 * 4,
	  .priority = (osPriority_t) osPriorityNormal,
};
void oLedTask(void *argument)
{
	OLEDDevice *poled =  OLEDDevGet();
	if(NULLDEV == poled)
	{
		;
	}
    
    osDelay(1000);
    if( poled->Init(poled)!= ESUCCESS )
        ;
	unsigned char buf[10] = {0};
	int count = 0;
	while (1)
	{
		sprintf(buf,"count %d",count);
		poled->ShowString(poled,1,1,buf);
		count++;
		osDelay(500);
	}
}

osThreadId_t led1Handle;
const osThreadAttr_t led1_attributes = {
	  .name = "led1",
	  .stack_size = 128 * 4,
	  .priority = (osPriority_t) osPriorityNormal,
};
void Led1Task(void *argument)
{
	GPIODevice *pled1 = GPIODevFind("LED1");
	if(NULLDEV == pled1)
	{
		;
	}

	while (1)
	{
		pled1->Write(pled1,0);
		osDelay(500);
		pled1->Write(pled1,1);
		osDelay(500);
	}
}

void multask_test()
{
	int ret = GPIODevRegister();
	if(ret != ESUCCESS)
		return;

	ret = I2CDevRegister();
	if(ret != ESUCCESS)
		return;
	
	oledHandle = osThreadNew(oLedTask, NULL, &oled_attributes);
	led1Handle = osThreadNew(Led1Task, NULL, &led1_attributes);
}


