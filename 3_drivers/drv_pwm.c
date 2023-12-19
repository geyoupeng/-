#include "errno.h"
#include "devices.h"
#include "config/pwm_config.h"
#include "tim.h"

static int PWMDrvInit(PWMDevice *ptdev);
static int PWMDrvStart(PWMDevice *ptdev);
static int PWMDrvStop(PWMDevice *ptdev);
static int PWMDrvGetCount(PWMDevice *ptdev);
static int PWMDrvControl(PWMDevice *ptdev,unsigned int cmd,unsigned int cfg);

//static void (*PWMIrqHandler[5])(void *arg);		//TimerIrqHandler[0]不存储	TimerIrqHandler[x]存储定时器x的中断回调函数

static PWMDevice gPWMDevices[] = {PWM3_1};


//获取TIMER设备头指针
PWMDevice * PWMGetDevHead()
{
	return gPWMDevices;
}

//获取TIMER设备数量
int PWMGetDevNum()
{
	return sizeof(gPWMDevices)/sizeof(PWMDevice);
}

//初始化PWM驱动
static int PWMDrvInit(PWMDevice *ptdev)
{
	if(NULL == ptdev)
		return -EINVAL;

	//设置中断回调	和一些默认配置
	switch(ptdev->port)
	{
		case 3:
			;
			break;
		default:
			break;
	}

	return ESUCCESS;
}

//开启PWM
static int PWMDrvStart(PWMDevice *ptdev)
{
	if(NULL == ptdev)
		return -EINVAL;

	//已经开启定时器不用重复开启
	if(1 == ptdev->status)
		return ESUCCESS;
	TIM_HandleTypeDef * htim = NULL;
	unsigned int channel = 0;
	
	//设置pwm通道 和 定时器句柄
	switch(ptdev->port)
	{
		case 3:
			htim = &htim3;
			channel = (ptdev->channel - 1) * 4;
			break;
		default:
			break;
	}

	if(NULL == htim)
	{
		return -EIO;
	}
	HAL_StatusTypeDef status = HAL_TIM_PWM_Start(htim, channel);
	if(status != HAL_OK)
	{
		//开启PWM失败
		return -EIO;
	}	

	//开启成功
	ptdev->status = 1;
	return ESUCCESS;
}

static int PWMDrvStop(PWMDevice *ptdev)
{
	if(NULL == ptdev)
		return -EINVAL;

	//已经关闭定时器不用重复关闭
	if(0 == ptdev->status)
		return ESUCCESS;
	
	TIM_HandleTypeDef * htim = NULL;
	unsigned int channel = 0;
	
	switch(ptdev->port)
	{
		case 3:
			htim = &htim3;
			channel = (ptdev->channel - 1) * 4;
			break;
		default:
			break;
	}

	if(NULL == htim)
	{
		return -EIO;
	}
	HAL_StatusTypeDef status = HAL_TIM_PWM_Stop(htim, channel);
	if(status != HAL_OK)
	{
		//关闭PWM
		return -EIO;
	}	

	//关闭成功
	__HAL_TIM_SetCounter(htim,0);
	ptdev->status = 0;
	return ESUCCESS;
}

static int PWMDrvGetCount(PWMDevice *ptdev)
{
	if(NULL == ptdev)
		return -EINVAL;

	unsigned int count = 0;
	
	switch(ptdev->port)
	{
		case 3:
			count = __HAL_TIM_GET_COUNTER(&htim3);
			break;
		default:
			break;
	}
	
	return count;
}

static int PWMDrvControl(PWMDevice *ptdev,unsigned int cmd,unsigned int cfg)
{
	
	if(NULL == ptdev)
		return -EINVAL;
	
	TIM_HandleTypeDef * htim = NULL;
	unsigned int frq = 0;
	unsigned int channel = 0;
	
	switch(ptdev->port)
	{
		case 3:
			htim = &htim3;
			channel = (ptdev->channel - 1) * 4;
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

			//frq = frq / (htim->Instance->PSC);
        {
			unsigned int max_int = 0xFFFFFFFF /frq + frq;
			unsigned int period = 0;
			
			if(cfg > max_int)
				period = cfg/1000*frq;
			else
				period = cfg*frq/1000;

			unsigned int prescaler = (period>>16) + 1;
			period  = period / prescaler;
			__HAL_TIM_SET_PRESCALER(htim, prescaler - 1);
			__HAL_TIM_SET_AUTORELOAD(htim, period - 1);
			break;
        }
		case SET_PULSE_VALUE:
        {
			unsigned int period = (htim->Instance->ARR + 1) * (htim->Instance->PSC + 1) / frq;	
			unsigned int pulse = cfg / 1000 * (htim->Instance->ARR + 1) / period ;				
			__HAL_TIM_SET_COMPARE(htim,channel,pulse);
			break;
        }
		default:
			break;
	}

	HAL_TIM_GenerateEvent(htim,TIM_EGR_UG);
    return ESUCCESS;
}

