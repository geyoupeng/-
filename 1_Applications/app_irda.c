#include "devices.h"
#include "mytype.h"
#include "FreeRTOS.h"
#include "task.h"
#include "errno.h"
#include "cmsis_os.h"
#include <string.h>
#include <stdio.h>

static TIMERDevice *ptim = NULL;
static GPIODevice *pbeep = NULL;

static void Timer2Irq(void *arg)
{
	pbeep->Write(pbeep,!pbeep->value);
}

//控制发声
void Sound(unsigned int frq,unsigned int ms)
{
		if(frq == 1000)
		{
			osDelay(1);
			return;
		}
		
        unsigned int time = 0;
		unsigned int second = ms *500;
		time = 10*10*10*10*10*10*10*10*10/frq/2;
		ptim->Control(ptim,SET_PERIOD_VALUE,time);
		ptim->Start(ptim);
		osDelay(second);
       	ptim->Stop(ptim);
		return;
}

//一闪一闪亮晶晶
void play_Music(void)
{
        //             低1  2   3   4   5   6   7  中1  2   3   4   5   6   7  高1    2   3     4    5    6    7  不发音
        unsigned int tone[] = {262,294,330,349,392,440,494,523,587,659,698,784,880,988,1047,1175,1319,1397,1568,1760,1967,1000};//音频数据表
        //              0   1   2   3   4   5   6   7   8   9   10  11  12  13  14   15   16    17   18   19   20  21
        //音调
        unsigned char music[]={1,1,5,5,6,6,5,4,4,3,3,2,2,1,
                                5,5,4,4,3,3,2,5,5,4,4,3,3,2,
                                1,1,5,5,6,6,5,4,4,3,3,2,2,1 
                                                        };        
        //节拍
        unsigned char time[] = {1,1,1,1,1,1,2,1,1,1,1,1,1,2,
                                1,1,1,1,1,1,2,1,1,1,1,1,1,2,
                                1,1,1,1,1,1,2,1,1,1,1,1,1,2
               };        
        unsigned int i;
        for(i=0;i<sizeof(music)/sizeof(music[0]);i++)//放歌
        {

              Sound(tone[music[i]],time[i]);   
        }
}

void BEEPTask(void *argument)
{
	pbeep = GPIODevFind("BEEP");
	if(NULLDEV == pbeep)
	{
		;
	}

	if(pbeep->Init(pbeep) != ESUCCESS)
	{
		;
	}	
	ptim = TIMERDevFind("TIMER2");
	if(NULLDEV == ptim)
	{
		;
	}
	ptim->IrqHandler = Timer2Irq;
	if(ptim->Init(ptim) != ESUCCESS)
	{
		;
	}
	ptim->Control(ptim,SET_PERIOD_VALUE,1000000);
	while (1)
	{
		 play_Music();
	}
}

static TaskHandle_t SoundTaskHandle = NULL;

void IRDA1Task(void *argument)
{
	OLEDDevice *poled =  OLEDDevGet();
	if(NULLDEV == poled)
	{
		;
	}
    
    osDelay(1000);
    if( poled->Init(poled)!= ESUCCESS )
        ;

	IDRADevice *irda = GetIRDADev();
	if(NULLDEV == irda)
	{
		;
	}
	if( irda->Init(irda)!= ESUCCESS )
        ;
	unsigned char buf[16] = {0};
	unsigned int taskstatus = 0;
    poled->ShowString(poled,1,1,"hello");
	while (1)
	{
		if( ESUCCESS==irda->Raed(irda) )
		{
			if(strstr(irda->key,"Suspend"))
			{
				poled->Clear(poled);
				poled->ShowString(poled,1,1,"sound Creat");
				if( NULL==SoundTaskHandle )
				{
					xTaskCreate(BEEPTask, "BEEPTask", 128, NULL, osPriorityNormal, &SoundTaskHandle);
					taskstatus = 1;
				}else
				{
					if(taskstatus)
					{
						poled->Clear(poled);
						poled->ShowString(poled,1,1,"sound suspend");
						vTaskSuspend(SoundTaskHandle);
						taskstatus = 0;
						ptim->Stop(ptim);
						pbeep->Write(pbeep,1);	
					}else
					{
						poled->Clear(poled);
						poled->ShowString(poled,1,1,"sound resume");
						vTaskResume(SoundTaskHandle);
						taskstatus = 1;
						ptim->Start(ptim);
					}
				}
			}
			if(strstr(irda->key,"Open"))
			{
				poled->Clear(poled);
				poled->ShowString(poled,1,1,"sound Delete");
				if(SoundTaskHandle)
				{
					vTaskDelete(SoundTaskHandle);
                    SoundTaskHandle = NULL;
                    ptim->Stop(ptim);
					pbeep->Write(pbeep,1);	
				}
			}
		}
	}
}

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

void irda_test()
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
	 ret = PWMDevRegister();
	if(ret != ESUCCESS)
		return;
	pbeep = GPIODevFind("BEEP");
	if(NULLDEV == pbeep)
	{
		;
	}
    pbeep->Write(pbeep,1);

    xTaskCreate(IRDA1Task, "IRDA Task", 128, NULL, osPriorityNormal, NULL);
    xTaskCreate(pwm_ledTask, "pwm_led", 128, NULL, osPriorityNormal, NULL);
}


