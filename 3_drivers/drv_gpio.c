#include "drv_gpio.h"
#include "gpio_config.h"
#include "devices.h"
#include "errno.h"
#include "string.h"
#include "stm32f1xx_hal.h"

static int GPIODrvInit(struct GPIODev *ptdev);
static int GPIODrvRead(struct GPIODev *ptdev);
static int GPIODrvWrite(struct GPIODev *ptdev,unsigned char value);

static GPIODevice gGPIOdevices[] = {S1,S2, IRDA, BEEP , LED1 , LED2 , PSCL , PSDA };

//获取GPIO设备头指针
GPIODevice * GPIOGetDevHead()
{
	return gGPIOdevices;
}

//获取GPIO设备数量
int GPIOGetDevNum()
{
	return sizeof(gGPIOdevices)/sizeof(GPIODevice);
}

EXTI_HandleTypeDef hexti1 = {
	.Line = EXTI_LINE_1,
	.PendingCallback = NULL
};

EXTI_HandleTypeDef hexti6 = {
	.Line = EXTI_LINE_6,
	.PendingCallback = NULL
};

static int GPIODrvInit(struct GPIODev *ptdev)
{
	//参数检查
	if(NULL == ptdev)
	{
		return -EINVAL;
	}

	EXTI_ConfigTypeDef  nConfig = {
		.Mode = EXTI_MODE_INTERRUPT,
		.Trigger = EXTI_TRIGGER_RISING_FALLING
	};
	if( strstr(ptdev->name,"IRDA") )
	{
		hexti1.PendingCallback = ptdev->IrqHandler;
		nConfig.Line = EXTI_LINE_1;
		nConfig.GPIOSel = EXTI_GPIOB;
		HAL_StatusTypeDef status = HAL_EXTI_SetConfigLine(&hexti1, &nConfig);
		if(status!=HAL_OK)
			return -EIO;
	}else if( strstr(ptdev->name,"S1") )
	{
		hexti6.PendingCallback = ptdev->IrqHandler;
		nConfig.Trigger = EXTI_TRIGGER_RISING;
		nConfig.Line = EXTI_LINE_6;
		nConfig.GPIOSel = EXTI_GPIOA;
		HAL_StatusTypeDef status = HAL_EXTI_SetConfigLine(&hexti6, &nConfig);
		if(status!=HAL_OK)
			return -EIO;
	}
	
	return ESUCCESS;
}

static int GPIODrvRead(struct GPIODev *ptdev)
{
	//参数检查
	if(NULL == ptdev)
	{
		return -EINVAL;
	}

	//使用HAl库读取引脚电平 存储在ptdev->value中
	ptdev->value = 	HAL_GPIO_ReadPin(ptdev->port, ptdev->pin);
	
	return ESUCCESS;
}

static int GPIODrvWrite(struct GPIODev *ptdev,unsigned char value)
{
	//参数检查
	if(NULL == ptdev)
	{
		return -EINVAL;
	}
	if( (value!=0) && (value!=1) )
	{
		return -EINVAL;
	}

	//使用HAl库向指定引脚输出电平
	HAL_GPIO_WritePin(ptdev->port, ptdev->pin,value);

	ptdev->value = value;
	return ESUCCESS;
}
