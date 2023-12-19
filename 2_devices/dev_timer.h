#ifndef __DEV_TIMER_H
#define __DEV_TIMER_H

/* 定义timer操作选项*/
typedef enum{
    SET_PRESCALER_VALUE = 1<<1,
    SET_CLKDIV_VALUE    = 1<<2,
    SET_PERIOD_VALUE    = 1<<3, // us unit
    GET_PERIOD_VALUE    = 1<<4,
    SET_PULSE_VALUE     = 1<<5,
    GET_PULSE_VALUE     = 1<<6,
}GeneralTimerCtrlItems;

//定时器设备对象
typedef struct TIMERDev{
	char *name;
	unsigned int status;
	unsigned int port;				//TIMERx
	unsigned int channel;			//TIMERx的通道y
	void (*IrqHandler)(void *arg);
	int (*Init)(struct TIMERDev *ptdev);
	int (*Start)(struct TIMERDev *ptdev);
	int (*Stop)(struct TIMERDev *ptdev);
	int (*GetCount)(struct TIMERDev *ptdev);
	int (*Control)(struct TIMERDev *ptdev,unsigned int cmd,unsigned int cfg);
	struct TIMERDev *next;
}TIMERDevice;

int TIMERDevRegister(void);
TIMERDevice *TIMERDevFind(char *name);

#endif /*__DEV_TIMER_H */

