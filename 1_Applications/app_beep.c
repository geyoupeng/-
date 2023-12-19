#include "devices.h"
#include "mytype.h"
#include "FreeRTOS.h"
#include "task.h"
#include "errno.h"
#include "cmsis_os.h"
#include "stm32f1xx_hal.h"
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
        HAL_Delay(second);
		//osDelay(second);
       	ptim->Stop(ptim);
		return;
}

//一闪一闪亮晶晶
void play_Music_2(void)
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


osThreadId_t beepHandle;
const osThreadAttr_t beep_attributes = {
	  .name = "beep",
	  .stack_size = 128 * 4,
	  .priority = (osPriority_t) osPriorityNormal,
};
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
    play_Music_2();
	while (1)
	{
		
	}
}

void beep_test()
{
	int ret = GPIODevRegister();
	if(ret != ESUCCESS)
		return;
	ret = TIMERDevRegister();
	if(ret != ESUCCESS)
		return;  
	
	beepHandle = osThreadNew(BEEPTask, NULL, &beep_attributes);

}

