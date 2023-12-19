#include "errno.h"
#include "devices.h"
#include "config/timer_config.h"
#include "tim.h"

static int TIMERDrvInit(struct TIMERDev *ptdev);
static int TIMERDrvStart(struct TIMERDev *ptdev);
static int TIMERDrvStop(struct TIMERDev *ptdev);
static int TIMERDrvGetCount(struct TIMERDev *ptdev);
static int TIMERDrvControl(struct TIMERDev *ptdev,unsigned int cmd,unsigned int cfg);

static void (*TimerIrqHandler[5])(void *arg);		//TimerIrqHandler[0]不存储	TimerIrqHandler[x]存储定时器x的中断回调函数
static volatile unsigned int gTIM1Count = 0;
static TIMERDevice gTimerDevices[] = {TIMER1DEV , TIMER2DEV};

//获取TIMER设备头指针
TIMERDevice * TIMERGetDevHead()
{
	return gTimerDevices;
}

//获取TIMER设备数量
int TIMERGetDevNum()
{
	return sizeof(gTimerDevices)/sizeof(TIMERDevice);
}

//初始化定时器
static int TIMERDrvInit(struct TIMERDev *ptdev)
{
	if(NULL == ptdev)
		return -EINVAL;

	//设置中断回调	和一些默认配置
	switch(ptdev->port)
	{
		case 1:
			gTIM1Count = 0;
			TimerIrqHandler[1] = ptdev->IrqHandler;
			break;
		case 2:
			TimerIrqHandler[2] = ptdev->IrqHandler;
			break;
		default:
			break;
	}

	return ESUCCESS;
}

//开启定时器
static int TIMERDrvStart(struct TIMERDev *ptdev)
{
	if(NULL == ptdev)
		return -EINVAL;

	//已经开启定时器不用重复开启
	if(1 == ptdev->status)
		return ESUCCESS;
	TIM_HandleTypeDef * htim = NULL;
	
	//设置中断回调	和一些默认配置
	switch(ptdev->port)
	{
		case 1:
			htim = &htim1;
			break;
		case 2:
			htim = &htim2;
			break;
		default:
			break;
	}

	if(NULL == htim)
	{
		return -EIO;
	}
	HAL_StatusTypeDef status = HAL_TIM_Base_Start_IT(htim);
	if(status != HAL_OK)
	{
		//定时器中断开启失败
		return -EIO;
	}	

	//开启成功
	ptdev->status = 1;
	return ESUCCESS;
}

static int TIMERDrvStop(struct TIMERDev *ptdev)
{
	if(NULL == ptdev)
		return -EINVAL;

	//已经关闭定时器不用重复关闭
	if(0 == ptdev->status)
		return ESUCCESS;
	
	TIM_HandleTypeDef * htim = NULL;
	
	switch(ptdev->port)
	{
		case 1:
			htim = &htim1;
			break;
		case 2:
			htim = &htim2;
			break;
		default:
			break;
	}

	if(NULL == htim)
	{
		return -EIO;
	}
	HAL_StatusTypeDef status = HAL_TIM_Base_Stop_IT(htim);
	if(status != HAL_OK)
	{
		//定时器中断开启失败
		return -EIO;
	}	

	//开启成功
	__HAL_TIM_SetCounter(htim,0);
	ptdev->status = 0;
	return ESUCCESS;
}

static int TIMERDrvGetCount(struct TIMERDev *ptdev)
{
	if(NULL == ptdev)
		return -EINVAL;

	unsigned int count = 0;
	
	switch(ptdev->port)
	{
		case 1:
			count = gTIM1Count;
			break;
		case 2:
			count = __HAL_TIM_GET_COUNTER(&htim2);
			break;
		default:
			break;
	}
	
	return count;
}

static int TIMERDrvControl(struct TIMERDev *ptdev,unsigned int cmd,unsigned int cfg)
{
	
	if(NULL == ptdev)
		return -EINVAL;
	
	TIM_HandleTypeDef * htim = NULL;
	unsigned int frq = 0;
	
	switch(ptdev->port)
	{
		case 1:
			htim = &htim1;
			frq = HAL_RCC_GetPCLK2Freq();
			break;
		case 2:
			htim = &htim2;
			frq = HAL_RCC_GetPCLK1Freq() * 2;
			break;
		default:
			break;
	}

	if(NULL == htim)
	{
		return -EIO;
	}
	frq = frq / 1000000;		//防止溢出 现在单位hz

	switch(cmd)
	{
		case SET_PRESCALER_VALUE:
			__HAL_TIM_SET_PRESCALER(htim, cfg);
			break;
		case SET_CLKDIV_VALUE:
			if(1 == cfg)
				__HAL_TIM_SET_CLOCKDIVISION(htim,TIM_CLOCKDIVISION_DIV1);
			else if(2 == cfg)
				__HAL_TIM_SET_CLOCKDIVISION(htim,TIM_CLOCKDIVISION_DIV2);
			else if(4 == cfg)
				__HAL_TIM_SET_CLOCKDIVISION(htim,TIM_CLOCKDIVISION_DIV4);
            break;
		case SET_PERIOD_VALUE:
        {
			unsigned int max_int = 0xFFFFFFFF/frq+frq;
			unsigned int period = 0;

			if(cfg > max_int)
				period = cfg/1000*frq;
			else
				period = cfg*frq/1000;

			unsigned int prescaler = (period>>16) + 1;
			period  = period / prescaler;
			__HAL_TIM_SET_PRESCALER(htim, prescaler-1);
			__HAL_TIM_SET_AUTORELOAD(htim, period-1);
			break;
        }
		default:
			break;
	}

	HAL_TIM_GenerateEvent(htim,TIM_EGR_UG);
    return ESUCCESS;
}

void TIMDrvIrq(TIM_HandleTypeDef *htim)
{
	void (*IrqHandler)(void *arg) = NULL;
	if(TIM1 == htim->Instance)
	{
		gTIM1Count++;
		IrqHandler = TimerIrqHandler[1];
	}
	if(TIM2 == htim->Instance)
	{
		IrqHandler = TimerIrqHandler[2];
	}

	if(IrqHandler != NULL)
		IrqHandler(NULL);
}

