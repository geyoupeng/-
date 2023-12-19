#ifndef __TIMER_CONFIG_H
#define __TIMER_CONFIG_H

//TIMER2 用来驱动蜂鸣器
#define TIMER2DEV	{				\
	.name = "TIMER2",				\
	.status = 0,					\
	.port = 2,						\
	.channel = 0,					\
	.IrqHandler = NULL,				\
	.Init = TIMERDrvInit,			\
	.Start = TIMERDrvStart,			\
	.Stop = TIMERDrvStop,			\
	.GetCount = TIMERDrvGetCount,	\
	.Control = TIMERDrvControl,	\
	.next = NULL					\
}									\

//TIMER1 用来给IRDA计数
#define TIMER1DEV	{				\
	.name = "TIMER1",				\
	.status = 0,					\
	.port = 1,						\
	.channel = 0,					\
	.IrqHandler = NULL,				\
	.Init = TIMERDrvInit,			\
	.Start = TIMERDrvStart,			\
	.Stop = TIMERDrvStop,			\
	.GetCount = TIMERDrvGetCount,	\
	.Control = TIMERDrvControl,	\
	.next = NULL					\
}									\

#endif /*__TIMER_CONFIG_H */

