#ifndef __DEV_PWM_H
#define __DEV_PWM_H
#include "dev_timer.h"

#define PWMDevice TIMERDevice

int PWMDevRegister(void);
PWMDevice *PWMDevFind(char *name);

#endif /*__DEV_PWM_H */

