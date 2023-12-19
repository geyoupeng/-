#ifndef __DEV_MPU6050_H
#define __DEV_MPU6050_H
#define MPUQUEUE 10
typedef struct MPU6050Data
{
	int anglex;
	int angley;
	int anglez;
}MPU6050Data;

typedef struct MPUData
{
	int dev;
	int angel;
}MPUData;

typedef struct MPU6050Dev{
	char *name;
	int (*Init)(struct MPU6050Dev *ptdev);
	int (*Read)(struct MPU6050Dev *ptdev, short *pAccX, short *pAccY, short *pAccZ, short *pGyroX, short *pGyroY, short *pGyroZ);
	int (*Parse)(struct MPU6050Dev *ptdev,short AccX, short AccY, short AccZ, short GyroX, short GyroY, short GyroZ,MPU6050Data *result);
}MPU6050Device;

MPU6050Device *MPU6050DevGet(void);
QueueHandle_t GetMPU6050Queue(void);

#endif /* __DEV_MPU6050_H */

