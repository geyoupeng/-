#include "devices.h"
#include "errno.h"
#include <math.h>

#define PWR_MGMT_1		0x6B
#define PWR_MGMT_2		0x6C
#define SMPLRT_DIV		0x19
#define CONFIG			0x1A
#define GYRO_CONFIG		0x1B
#define ACCEL_CONFIG	0x1C
#define WHO_AM_I		0x75 

#define	MPU6050_ACCEL_XOUT_H	0x3B
#define	MPU6050_ACCEL_XOUT_L	0x3C
#define	MPU6050_ACCEL_YOUT_H	0x3D
#define	MPU6050_ACCEL_YOUT_L	0x3E
#define	MPU6050_ACCEL_ZOUT_H	0x3F
#define	MPU6050_ACCEL_ZOUT_L	0x40
#define	MPU6050_TEMP_OUT_H		0x41
#define	MPU6050_TEMP_OUT_L		0x42
#define	MPU6050_GYRO_XOUT_H		0x43
#define	MPU6050_GYRO_XOUT_L		0x44
#define	MPU6050_GYRO_YOUT_H		0x45
#define	MPU6050_GYRO_YOUT_L		0x46
#define	MPU6050_GYRO_ZOUT_H		0x47
#define	MPU6050_GYRO_ZOUT_L		0x48

/* 传感器数据修正值（消除芯片固定误差，根据硬件进行调整） */
#define MPU6050_X_ACCEL_OFFSET	(-64) 
#define MPU6050_Y_ACCEL_OFFSET 	(-30)
#define MPU6050_Z_ACCEL_OFFSET 	(14400) 
#define MPU6050_X_GYRO_OFFSET 	(40)
#define MPU6050_Y_GYRO_OFFSET 	(-7)
#define MPU6050_Z_GYRO_OFFSET 	(-14)

static int MPU6050Init(struct MPU6050Dev *ptdev);

static int MPU6050Read(struct MPU6050Dev *ptdev,short *pAccX,short *pAccY,short *pAccZ, short *pGyroX, short *pGyroY,short *pGyroZ);
static int MPU6050ParseData(struct MPU6050Dev *ptdev,short AccX, short AccY, short AccZ, short GyroX,  short GyroY, short GyroZ,MPU6050Data *result);

static I2CDevice *pi2c = NULL;
static QueueHandle_t gmpuqueue = NULL;

MPU6050Device gmpu6050 = {
	.name = "mpu6050",
	.Init = MPU6050Init,
	.Read = MPU6050Read,
	.Parse = MPU6050ParseData
};

QueueHandle_t GetMPU6050Queue()
{
	return gmpuqueue;
}

MPU6050Device *MPU6050DevGet()
{
	return &gmpu6050;
}

static int GetId()
{
	unsigned char buf = 0;
	
	//读取MPU6050ID
	int len = pi2c->Read(pi2c,&buf,1,WHO_AM_I);
	if(len != 1)
		return -EIO;

	if(buf!=0x68)
		return -EIO;
	
	return ESUCCESS;
}
static int MPU6050Init(struct MPU6050Dev *ptdev)
{
	//参数检查
	if(NULL == ptdev)
		return -EINVAL;

	//初始化i2c
	pi2c = I2CDevFind("I2C2");
	if(NULL == pi2c)
		return -EIO;

	pi2c->slave_addr = 0xD0;
	if(pi2c->Init(pi2c)!=ESUCCESS)
		return -EIO;

	unsigned char buf = 0;
    int len = 0;
   
	//关闭睡眠模式	设置系统时钟为内部时钟
	buf = 0x00;
    len = pi2c->Write(pi2c,&buf,1,PWR_MGMT_1);
	if(len != 1)
		return -EIO;

    buf = 0x00;
    len = pi2c->Write(pi2c,&buf,1,PWR_MGMT_2);
    
	//设置采样率
	buf = 0x09;
	len = pi2c->Write(pi2c,&buf,1,SMPLRT_DIV);
	if(len != 1)
		return -EIO;

	//打开低通滤波 设置陀螺仪输出率为1k
	buf = 0x06;
	len = pi2c->Write(pi2c,&buf,1,CONFIG);
	if(len != 1)
		return -EIO;

	//关闭陀螺仪自检 使能陀螺仪
	buf = 0x18;
	len = pi2c->Write(pi2c,&buf,1,GYRO_CONFIG);
	if(len != 1)
		return -EIO;

	//关闭加速度计自检 使能加速度计
	buf = 0x18;
	len = pi2c->Write(pi2c,&buf,1,ACCEL_CONFIG);
	if(len != 1)
		return -EIO;

	if(GetId()!=ESUCCESS)
		return -EIO;

	gmpuqueue = xQueueCreate(10, sizeof(MPUData));	
	return ESUCCESS;
}

static int MPU6050ParseData(struct MPU6050Dev *ptdev,short AccX, short AccY, short AccZ, short GyroX, short GyroY, short GyroZ, struct MPU6050Data *result)
{
	if (result)
	{
		result->anglex = (int)(acos((double)((double)(AccX + MPU6050_X_ACCEL_OFFSET) / 16384.0)) * 57.29577);

		result->angley = (int)(acos((double)((double)(AccY + MPU6050_Y_ACCEL_OFFSET) / 16384.0)) * 57.29577)+1;
	}
	return ESUCCESS;
}

static int MPU6050Read(struct MPU6050Dev *ptdev,short *pAccX, short *pAccY, short *pAccZ, short *pGyroX, short *pGyroY, short *pGyroZ)
{
	unsigned char datal = 0, datah = 0;
    int len = 0;

    len = pi2c->Read(pi2c,&datah,1,MPU6050_ACCEL_XOUT_H);
    len = pi2c->Read(pi2c,&datal,1,MPU6050_ACCEL_XOUT_L);
	if(pAccX)
        *pAccX = (datah << 8) | datal;

    
	len = pi2c->Read(pi2c,&datah,1,MPU6050_ACCEL_YOUT_H);
	len = pi2c->Read(pi2c,&datal,1,MPU6050_ACCEL_YOUT_L);
	if(pAccY)
        *pAccY = (datah << 8) | datal;

	len = pi2c->Read(pi2c,&datah,1,MPU6050_ACCEL_ZOUT_H);
	len = pi2c->Read(pi2c,&datal,1,MPU6050_ACCEL_ZOUT_L);
	if(pAccZ)
        *pAccZ = (datah << 8) | datal;

	len = pi2c->Read(pi2c,&datah,1,MPU6050_GYRO_XOUT_H);
	len = pi2c->Read(pi2c,&datal,1,MPU6050_GYRO_XOUT_L);
	if(pGyroX)
        *pGyroX = (datah << 8) | datal;

	len = pi2c->Read(pi2c,&datah,1,MPU6050_GYRO_YOUT_H);
	len = pi2c->Read(pi2c,&datal,1,MPU6050_GYRO_YOUT_L);
	if(pGyroY)
        *pGyroY = (datah << 8) | datal;

	len = pi2c->Read(pi2c,&datah,1,MPU6050_GYRO_ZOUT_H);
	len = pi2c->Read(pi2c,&datal,1,MPU6050_GYRO_ZOUT_L);
	if(pGyroZ)
        *pGyroZ = (datah << 8) | datal;
    
    return len;
}

