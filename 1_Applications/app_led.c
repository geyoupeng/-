#include "devices.h"
#include "mytype.h"
#include "FreeRTOS.h"
#include "task.h"
#include "errno.h"
#include "cmsis_os.h"

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

osThreadId_t led2Handle;
const osThreadAttr_t led2_attributes = {
	  .name = "led2",
	  .stack_size = 128 * 4,
	  .priority = (osPriority_t) osPriorityNormal,
};
void Led2Task(void *argument)
{
	GPIODevice *pled2 = GPIODevFind("LED2");
	if(NULLDEV == pled2)
	{
		;
	}

	while (1)
	{
		pled2->Write(pled2,0);
		osDelay(1000);
		pled2->Write(pled2,1);
		osDelay(1000);
	}
}

void led_test()
{
	int ret = GPIODevRegister();
	if(ret != ESUCCESS)
		return;
	  
	led1Handle = osThreadNew(Led1Task, NULL, &led1_attributes);
    led2Handle = osThreadNew(Led2Task, NULL, &led2_attributes);
}
