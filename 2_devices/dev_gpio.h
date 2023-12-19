#ifndef __DEV_GPIO_H
#define __DEV_GPIO_H

typedef struct GPIODev{
	char *name;
    unsigned char value;
	void *port;
	unsigned int pin;
	int (*Init)(struct GPIODev *ptdev);
	int (*Read)(struct GPIODev *ptdev);
	int (*Write)(struct GPIODev *ptdev,unsigned char value);
	void (*IrqHandler)(void);
	struct GPIODev *next;
}GPIODevice;

int GPIODevRegister(void);
GPIODevice *GPIODevFind(char *name);

#endif /*__DEV_GPIO_H */

