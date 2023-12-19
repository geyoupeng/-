#ifndef __CONFIG_H
#define __CONFIG_H

/* 管理驱动对象是否使用 */
#define USE_GPIO_DRIVER     (1)
#define USE_I2C_DRIVER     	(1)
#define USE_TIMER_DRIVER    (1)
#define USE_PWM_DRIVER    	(1)

/* 管理设备对象是否使用 */
#define USE_OLED_MODULE     (1)
#define USE_IRDA_MODULE     (1)
#define USE_ROTARY_MODULE   (1)
#define USE_MPU6050_MODULE   (1)

/* 管理库文件是否使用 */
#define USE_LIB     (1)

#endif /* __CONFIG_H */
