#include "devices.h"
#include "mytype.h"
#include "FreeRTOS.h"
#include "task.h"
#include "errno.h"
#include "cmsis_os.h"
#include "draw.h"

void DrawTask(void *argument)
{
	DrawInit();
	DrawString(0, 0,"nihao wojojiao");
	while (1)
	{
		
	}
}

void draw_test()
{
	int ret = GPIODevRegister();
	if(ret != ESUCCESS)
		return;
	ret = I2CDevRegister();
	if(ret != ESUCCESS)
		return;
	
	xTaskCreate(DrawTask,"DrawTask", 128, NULL, osPriorityNormal, NULL);
}


