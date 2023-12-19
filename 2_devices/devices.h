#ifndef __DEVICES_H
#define __DEVICES_H
#include "drivers.h"

/* 管理GPIO设备 */
#ifdef USE_GPIO_DEVICE
	#include "dev_gpio.h"
#endif

/* 管理I2C设备 */
#ifdef USE_I2C_DEVICE
	#include "dev_i2c.h"
#endif

/* 管理TIMER设备 */
#ifdef USE_TIMER_DEVICE
	#include "dev_timer.h"
#endif

/* 管理PWM设备 */
#ifdef USE_PWM_DEVICE
	#include "dev_pwm.h"
#endif

/* 管理OLED设备 */
#ifdef USE_OLED_MODULE
	#include "oled/dev_oled.h"
#endif

/* 管理IRDA设备 */
#ifdef USE_IRDA_MODULE
	#include "irda/dev_irda.h"
#endif

/* 管理ROTATY设备 */
#ifdef USE_ROTARY_MODULE
	#include "rotary/dev_rotary.h"
#endif

/* 管理MPU6050设备 */
#ifdef USE_MPU6050_MODULE
	#include "mpu6050/dev_mpu6050.h"
#endif

#endif /* __DEVICES_H */

