#ifndef __I2C_CONFIG_H
#define __I2C_CONFIG_H

//I2C1配置	用来驱动OLED显示
#define I2C1DEV	{				\
	.name = "I2C1",				\
	.channel = 1,				\
	.own_addr = 0,				\
	.slave_addr = 0,			\
	.Init = I2CDrvInit,			\
	.Read = I2CDrvRead,			\
	.Write = I2CDrvWrite,		\
	.next = NULL				\
}								\

//I2C2配置	用来驱动满铺050显示
#define I2C2DEV	{				\
	.name = "I2C2",				\
	.channel = 2,				\
	.own_addr = 0,				\
	.slave_addr = 0,			\
	.Init = I2CDrvInit,			\
	.Read = I2CDrvRead,			\
	.Write = I2CDrvWrite,		\
	.next = NULL				\
}								\

#endif /*__I2C_CONFIG_H */

