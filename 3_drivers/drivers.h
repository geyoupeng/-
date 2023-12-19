#ifndef __DRIVERS_H
#define __DRIVERS_H
#include "config.h"

/* 管理GPIO驱动 */
#ifdef USE_GPIO_DRIVER
	#include "drv_gpio.h"
	#define USE_GPIO_DEVICE     (1)
#endif

/* 管理I2C驱动 */
#ifdef USE_I2C_DRIVER
	#include "drv_i2c.h"
	#define USE_I2C_DEVICE     (1)
#endif

/* 管理TIMER驱动 */
#ifdef USE_TIMER_DRIVER
	#include "drv_timer.h"
	#define USE_TIMER_DEVICE     (1)
#endif

/* 管理PWM驱动 */
#ifdef USE_PWM_DRIVER
	#include "drv_pwm.h"
	#define USE_PWM_DEVICE     (1)
#endif

#endif /* __DRIVERS_H */

