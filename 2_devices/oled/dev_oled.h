#ifndef __DEV_OLED_H
#define __DEV_OLED_H

#define FRAME_WIDTH 128
#define FRAME_HEIGTH 64

//OLED设备对象
typedef struct OLEDDev{
	char *name;
	int (*Init)(struct OLEDDev *ptdev);
	int (*DisplayOn)(struct OLEDDev *ptdev);
	int (*DisplayOff)(struct OLEDDev *ptdev);
	int (*ShowString)(struct OLEDDev *ptdev,unsigned char x,unsigned char y,unsigned char *str);
	int (*Clear)(struct OLEDDev *ptdev);
	int (*FlushRegion)(struct OLEDDev *ptdev,unsigned char col, unsigned char row, unsigned char width, unsigned char heigth);
}OLEDDevice;

OLEDDevice *OLEDDevGet(void);
unsigned char *OLEDBufferGet(void);

#endif /*__DEV_OLED_H */

