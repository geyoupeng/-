#include "devices.h"
#include "mytype.h"
#include "FreeRTOS.h"
#include "task.h"
#include "errno.h"
#include "cmsis_os.h"


osThreadId_t pwm_ledHandle;
const osThreadAttr_t pwm_led_attributes = {
	  .name = "pwm_led",
	  .stack_size = 128 * 4,
	  .priority = (osPriority_t) osPriorityNormal,
};
void pwm_ledTask(void *argument)
{
	PWMDevice *ppwm = PWMDevFind("PWM3_1");
	if(NULLDEV == ppwm)
	{
		;
	}
	if( ppwm->Init(ppwm)!=ESUCCESS )
		return;
	//if(ppwm->Control(ppwm,SET_PRESCALER_VALUE,72)!=ESUCCESS )
	//	return;
    unsigned int period = 1000000;
	if(ppwm->Control(ppwm,SET_PERIOD_VALUE,period)!=ESUCCESS )
		return;
    //ppwm->Control(ppwm,SET_PULSE_VALUE,period);
	ppwm->Start(ppwm);
    //ppwm->Control(ppwm,SET_PULSE_VALUE,0);
	//unsigned int pulse = 0;
	unsigned int count = 0;
    unsigned int flag = 0;
	while (1)
	{
        ppwm->Control(ppwm,SET_PULSE_VALUE,count);
        if(!flag)
        {
            count = count + 1000;
            if(count == period)
                flag = 1;
        }else
        {
            count = count - 1000;
            if(count == 0)
                flag = 0;
        }
		osDelay(1);
	}
}

void pwm_led_test()
{
	//int ret = GPIODevRegister();
	//if(ret != ESUCCESS)
	//	return;
	int ret = PWMDevRegister();
	if(ret != ESUCCESS)
		return;
	
	pwm_ledHandle = osThreadNew(pwm_ledTask, NULL, &pwm_led_attributes);
}

