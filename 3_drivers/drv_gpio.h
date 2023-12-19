#ifndef __DRV_GPIO_H
#define __DRV_GPIO_H
#include "dev_gpio.h"
GPIODevice * GPIOGetDevHead(void);

//获取GPIO设备数量
int GPIOGetDevNum(void);

#endif /*__DRV_GPIO_H */

