#include "devices.h"
#include "errno.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "stm32f1xx_hal.h"
#include "mytype.h"

static int RotaryDevInit(struct ROTARYDev* ptdev);
static int RotaryDevRead(struct ROTARYDev* ptdev,int *count,int *speed);
static GPIODevice * ps2 = NULL;
static GPIODevice * ps1 = NULL;

static ROTARYDevice grotary = {
									.name = "Rotary",
									.count = 0,
									.speed = 0,
									.Init = RotaryDevInit,	
									.Read = RotaryDevRead,
};

ROTARYDevice *GetROTARYDevice()
{
	return &grotary;
}

unsigned long long system_get_ns(void)
{
    //extern uint32_t HAL_GetTick(void);
    extern TIM_HandleTypeDef        htim4;
    TIM_HandleTypeDef *hHalTim = &htim4;
    
    unsigned long long ns = HAL_GetTick();
    unsigned long long cnt;
    unsigned long long reload;

    cnt = __HAL_TIM_GET_COUNTER(hHalTim);
    reload = __HAL_TIM_GET_AUTORELOAD(hHalTim);

    ns *= 1000000;
    ns += cnt * 1000000 / reload;
    return ns;
}

static QueueHandle_t grotaryqueue = NULL;

QueueHandle_t GetROTARYQueue()
{
	return grotaryqueue;
}

static RotartData mydata = {0};
void RotaryEncode()
{
	unsigned long long time = 0;
	static unsigned long long pretime = 0; 
 	osDelay(2);
	if(ps1->Read(ps1)==ESUCCESS)
	{
		if(!ps1->value) 
			return;
	}
	time = system_get_ns();
	
	grotary.speed = (unsigned long long)1000000000/(time-pretime);
	if(ps2->Read(ps2)==ESUCCESS)
	{
		if(!ps2->value)
		{
			grotary.count++;
		}else
		{
			grotary.count--;
			grotary.speed = 0 - grotary.speed;
		}
	}
	pretime = time;

	mydata.count = grotary.count;
	mydata.speed = grotary.speed;
	xQueueSendFromISR(grotaryqueue,&mydata,NULL);
}

static int RotaryDevRead(struct ROTARYDev* ptdev,int *count,int *speed)
{
	if(NULL == ptdev)
		return -EINVAL;
	static int precount = 0;
	*count = ptdev->count;
	if(ptdev->count == precount)
	{
		*speed = 0;
		 ptdev->speed = 0;
	}else
	{
		*speed = ptdev->speed;
	}
	return ESUCCESS;
}

static int RotaryDevInit(struct ROTARYDev* ptdev)
{
	if(NULL == ptdev)
		return -EINVAL;
	//初始化 S1 和S2
	ps1 = GPIODevFind("S1");
	if(NULL == ps1)
	{
		return -EIO;
	}
	ps1->IrqHandler =RotaryEncode;
	if(ps1->Init(ps1)!=ESUCCESS)
	{
		return -EIO;
	}
	ps2 = GPIODevFind("S2");
	if(NULL == ps2)
	{
		return -EIO;
	}

	grotaryqueue =  xQueueCreate(10, sizeof(RotartData));
	return ESUCCESS;
}

