#ifndef __DEV_ROTARY_H
#define __DEV_ROTARY_H
#define ROTARYQUEUE	10
#include "FreeRTOS.h"
#include "queue.h"
typedef struct ROTARYDev{
	unsigned char *name;
	int count;
	int speed;
	int (*Init)(struct ROTARYDev* ptdev);
	int (*Read)(struct ROTARYDev* ptdev,int *count,int *speed);
}ROTARYDevice;

typedef struct RotartData_t
{
	int count;
	int speed;
}RotartData;

ROTARYDevice *GetROTARYDevice(void);
QueueHandle_t GetROTARYQueue(void);

#endif /* __DEV_ROTARY_H*/
