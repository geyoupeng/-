#ifndef __DEV_IRDA_H
#define __DEV_IRDA_H
#define IRDAQUEUE	10
#include "FreeRTOS.h"
#include "queue.h"
//红外接收对象设备结构体s
typedef struct IDRADev{
	char *name;
	char *key;
	int value;
	int (*Init)(struct IDRADev *ptdev);
	int (*Raed)(struct IDRADev *ptdev);		
}IDRADevice;

typedef struct IrdaData_t
{
	int dev;
	int data;
}IrdaData;

IDRADevice *GetIRDADev(void);
QueueHandle_t GetIRDAQueue(void);

#endif /* __DEV_IRDA_H */

