#ifndef __DRV_PWM_H
#define __DRV_PWM_H
#include "dev_pwm.h"

PWMDevice * PWMGetDevHead(void);

//获取TIMER设备数量
int PWMGetDevNum(void);

#endif /* __DRV_PWM_H */

