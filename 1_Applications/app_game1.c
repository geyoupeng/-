#include "devices.h"
#include "mytype.h"
#include "FreeRTOS.h"
#include "task.h"
#include "errno.h"
#include "cmsis_os.h"
#include "draw.h"

extern void Game1Task(void *arg);

void game1_test()
{
	int ret = GPIODevRegister();
	if(ret != ESUCCESS)
		return;
	ret = TIMERDevRegister();
	if(ret != ESUCCESS)
		return;
	
	ret = I2CDevRegister();
	if(ret != ESUCCESS)
		return;

    xTaskCreate(Game1Task, "Game1Task", 128, NULL, osPriorityNormal, NULL);
}

