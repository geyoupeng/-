#include "devices.h"
#include "mytype.h"
#include "FreeRTOS.h"
#include "task.h"
#include "errno.h"
#include "cmsis_os.h"
#include "draw.h"

#if 0
void RotaryTask(void *argument)
{
	ROTARYDevice * protary = GetROTARYDevice();
	if(NULLDEV == protary)
		;
	if(protary->Init(protary)!=ESUCCESS)
		;

	int count = 0;
	int speed = 0;
	DrawInit();
	DrawString(0, 0, "RotaryEncoder: ");

   	char buf[16] = {0};
	while (1)
	{
		 protary->Read(protary,&count, &speed);
		 sprintf(buf,"Cnt  : %d",count);
		 DrawString(0, 8, buf);
		 sprintf(buf,"Speed: %d",speed);
		 DrawString(0, 16, buf);
	}
}
#endif
void rotary_test()
{
	int ret = GPIODevRegister();
	if(ret != ESUCCESS)
		return;
	ret = I2CDevRegister();
	if(ret != ESUCCESS)
		return;
	
	//xTaskCreate(RotaryTask,"RotaryTask", 128, NULL, osPriorityNormal, NULL);
}



