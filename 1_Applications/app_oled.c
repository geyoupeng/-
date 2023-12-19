#include "devices.h"
#include "mytype.h"
#include "FreeRTOS.h"
#include "task.h"
#include "errno.h"
#include "cmsis_os.h"
#include <string.h>
#include <stdio.h>

typedef struct PrintInfo_t
{
	unsigned char line;
	unsigned char col;
	char name[16];
}PrintInfo;

static PrintInfo Task1Info = {1,1,"Task1:"};
static PrintInfo Task2Info = {2,1,"Task2:"};
static PrintInfo Task3Info = {3,1,"Task3:"};
static unsigned int OLEDUSE = 1;
void oLedTask(void *argument)
{
	PrintInfo * arg = (PrintInfo *)argument;
	OLEDDevice *poled =  OLEDDevGet();
	if(NULLDEV == poled)
	{
		;
	}
	int count = 0;
	while (1)
	{
		if(OLEDUSE)
		{
			OLEDUSE = 0;
            sprintf(&arg->name[6],"%d",count++);
			poled->ShowString(poled,arg->line,arg->col,arg->name);
 			OLEDUSE = 1;
		}
		osDelay(500);
	}
}

void oLedTask1(void *argument)
{
	PrintInfo * arg = (PrintInfo *)argument;
	OLEDDevice *poled =  OLEDDevGet();
	if(NULLDEV == poled)
	{
		;
	}
	int count = 0;
	while (1)
	{
		if(OLEDUSE)
		{
			OLEDUSE = 0;
            sprintf(&arg->name[6],"%d",count++);
			poled->ShowString(poled,arg->line,arg->col,arg->name);
 			OLEDUSE = 1;
		}
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
	OLEDDevice *poled =  OLEDDevGet();
	if(NULLDEV == poled)
	{
		;
	}
    
    osDelay(1000);
    if( poled->Init(poled)!= ESUCCESS )
        ;
	poled->ShowString(poled,1,1,"hello");
    poled->Clear(poled);
	xTaskCreate(oLedTask, "Task1", 128, &Task1Info, osPriorityNormal, NULL);
	xTaskCreate(oLedTask, "Task2", 128, &Task2Info, osPriorityNormal, NULL);
    xTaskCreate(oLedTask1, "Task3", 128, &Task3Info, osPriorityNormal, NULL);
}

