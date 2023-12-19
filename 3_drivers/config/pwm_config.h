#ifndef __PWM_CONFIG_H
#define __PWM_CONFIG_H

//PWM3-1 用来驱动led
#define PWM3_1	{					\
	.name = "PWM3_1",				\
	.status = 0,					\
	.port = 3,						\
	.channel = 1,					\
	.IrqHandler = NULL,				\
	.Init = PWMDrvInit,				\
	.Start = PWMDrvStart,			\
	.Stop = PWMDrvStop,				\
	.GetCount = PWMDrvGetCount,		\
	.Control = PWMDrvControl,		\
	.next = NULL					\
}									\

#endif /*__PWM_CONFIG_H */


