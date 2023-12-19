#ifndef __DEV_I2C_H
#define __DEV_I2C_H

typedef struct I2CDev{
	char *name;
	unsigned int channel;
	unsigned short own_addr;
	unsigned short slave_addr;
	int (*Init)(struct I2CDev *ptdev);
	int (*Read)(struct I2CDev *ptdev,unsigned char *buf,unsigned int length,unsigned char reg);
	int (*Write)(struct I2CDev *ptdev,unsigned char *buf,unsigned int length,unsigned char reg);
	struct I2CDev *next;
}I2CDevice;

int I2CDevRegister(void);
I2CDevice *I2CDevFind(char *name);

#endif /*__DEV_I2C_H */

