#include "drv_i2c.h"
#include "i2c_config.h"
#include "devices.h"
#include "errno.h"
#include "i2c.h"
#include "stm32f1xx_hal.h"

static int I2CDrvInit(struct I2CDev *ptdev);
static int I2CDrvRead(struct I2CDev *ptdev,unsigned char *buf,unsigned int length,unsigned char reg);
static int I2CDrvWrite(struct I2CDev *ptdev,unsigned char *buf,unsigned int length,unsigned char reg);

static I2CDevice gI2Cdevices[] = { I2C1DEV , I2C2DEV};

//获取I2C设备头指针
I2CDevice * I2CGetDevHead()
{
	return gI2Cdevices;
}

//获取I2C设备数量
int I2CGetDevNum()
{
	return sizeof(gI2Cdevices)/sizeof(I2CDevice);
}

static int I2CDrvInit(struct I2CDev *ptdev)
{
	//参数检查
	if(NULL == ptdev)
	{
		return -EINVAL;
	}

	I2C_HandleTypeDef *hi2c = NULL;
	if(1 == ptdev->channel)
	{
		hi2c = &hi2c1;
	}else if(2 == ptdev->channel)
	{
		hi2c = &hi2c2;
	}else
	{
		hi2c = NULL;
	}

	if(NULL == hi2c)
		return -EIO;

	ptdev->own_addr = hi2c->Init.OwnAddress1;
	return ESUCCESS;
}

static int I2CDrvWrite(struct I2CDev *ptdev,unsigned char *buf,unsigned int length,unsigned char reg)
{
	//参数检查
	if(NULL == ptdev)
	{
		return -EINVAL;
	}
	if(NULL == buf)
	{
		return -EINVAL;
	}
	if(0 == length)
	{
		return -EINVAL;
	}

	I2C_HandleTypeDef *hi2c = NULL;
	if(1 == ptdev->channel)
	{
		hi2c = &hi2c1;
	}else if(2 == ptdev->channel)
	{
		hi2c = &hi2c2;
	}else
	{
		hi2c = NULL;
	}

	if(NULL == hi2c)
		return -EIO;

    unsigned init_int = length;
	while(length)
	{
		unsigned char *pbuf = buf;
		unsigned int size = 0;

		if(length > 0xFF)
		{
			size = 0xFF;
		}else
		{
			size = length;
		}

        HAL_StatusTypeDef status;
        status = HAL_I2C_Mem_Write(hi2c, ptdev->slave_addr,reg,1 ,pbuf,size, 0xff);    
		if(status != HAL_OK)
			return -EIO;
		
		length = length -size;
		pbuf = pbuf+size;
	}

	return init_int;
}

static int I2CDrvRead(struct I2CDev *ptdev,unsigned char *buf,unsigned int length,unsigned char reg)
{
	//参数检查
	if(NULL == ptdev)
	{
		return -EINVAL;
	}
	if(NULL == buf)
	{
		return -EINVAL;
	}
	if(0 == length)
	{
		return -EINVAL;
	}

	I2C_HandleTypeDef *hi2c = NULL;
	if(1 == ptdev->channel)
	{
		hi2c = &hi2c1;
	}else if(2 == ptdev->channel)
	{
		hi2c = &hi2c2;
	}else
	{
		hi2c = NULL;
	}


    unsigned init_int = length;
	while(length)
	{
		unsigned char *pbuf = buf;
		unsigned int size = 0;

		if(length > 0xFF)
		{
			size = 0xFF;
		}else
		{
			size = length;
		}

        HAL_StatusTypeDef status = HAL_I2C_Mem_Read(hi2c, ptdev->slave_addr,reg,1 ,pbuf,size, 0xff);    

		if(status != HAL_OK)
			return -EIO;
		length = length -size;
		pbuf = pbuf+size;
	}

	return init_int;
}

