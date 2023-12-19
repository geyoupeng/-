#ifndef __GPIO_CONFIG_H
#define __GPIO_CONFIG_H

//旋转编码器S1 配置
#define S1	{				\
	.name = "S1",			\
	.value = 0,				\
	.port = GPIOA,			\
	.pin = GPIO_PIN_6,		\
	.Init = GPIODrvInit,	\
	.Read = GPIODrvRead,	\
	.Write = NULL,			\
	.IrqHandler = NULL,		\
	.next = NULL			\
}							\

//旋转编码器S2 配置
#define S2	{				\
	.name = "S2",			\
	.value = 0,				\
	.port = GPIOA,			\
	.pin = GPIO_PIN_8,		\
	.Init = GPIODrvInit,	\
	.Read = GPIODrvRead,	\
	.Write = NULL,			\
	.IrqHandler = NULL,		\
	.next = NULL			\
}							\

//IRDA 配置
#define IRDA	{			\
	.name = "IRDA",			\
	.value = 0,				\
	.port = GPIOB,			\
	.pin = GPIO_PIN_1,		\
	.Init = GPIODrvInit,	\
	.Read = GPIODrvRead,	\
	.Write = GPIODrvWrite,	\
	.IrqHandler = NULL,		\
	.next = NULL			\
}							\

//beep 配置
#define BEEP	{			\
	.name = "BEEP",			\
	.value = 0,				\
	.port = GPIOA,			\
	.pin = GPIO_PIN_7,		\
	.Init = GPIODrvInit,	\
	.Read = GPIODrvRead,	\
	.Write = GPIODrvWrite,	\
	.IrqHandler = NULL,		\
	.next = NULL			\
}							\

//led1 配置
#define LED1	{			\
	.name = "LED1",			\
	.value = 1,				\
	.port = GPIOA,			\
	.pin = GPIO_PIN_0,		\
	.Init = GPIODrvInit,	\
	.Read = GPIODrvRead,	\
	.Write = GPIODrvWrite,	\
	.IrqHandler = NULL, 	\
	.next = NULL			\
}							\


//led2 配置
#define LED2	{			\
	.name = "LED2",			\
	.value = 1,				\
	.port = GPIOA,			\
	.pin = GPIO_PIN_1,		\
	.Init = GPIODrvInit,	\
	.Read = GPIODrvRead,	\
	.Write = GPIODrvWrite,	\
	.IrqHandler = NULL, 	\
	.next = NULL			\
}							\


//PSCL 配置
#define PSCL	{			\
	.name = "PB8",			\
	.value = 1,				\
	.port = GPIOB,			\
	.pin = GPIO_PIN_8,		\
	.Init = GPIODrvInit,	\
	.Read = GPIODrvRead,	\
	.Write = GPIODrvWrite,	\
	.IrqHandler = NULL, 	\
	.next = NULL			\
}							\


//PSDA 配置
#define PSDA	{			\
	.name = "PB9",			\
	.value = 1,				\
	.port = GPIOB,			\
	.pin = GPIO_PIN_9,		\
	.Init = GPIODrvInit,	\
	.Read = GPIODrvRead,	\
	.Write = GPIODrvWrite,	\
	.IrqHandler = NULL, 	\
	.next = NULL			\
}							\

#endif /*__GPIO_CONFIG_H */

