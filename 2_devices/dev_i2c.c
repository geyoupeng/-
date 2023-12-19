#include "errno.h"
#include "mytype.h"
#include "drivers.h"
#include "string.h"

//I2C设备链表头指针
static I2CDevice *I2CDevHead = NULLDEV;

//将GPIO设备节点插入 GPIO设备链表中
int I2CDevInsert(I2CDevice *ptdev)
{
	if(NULLDEV == ptdev)
	{
		return -EINVAL;
	}

	if(NULLDEV == I2CDevHead)
	{
		I2CDevHead = ptdev;
	}else
	{
		ptdev->next = I2CDevHead;
		I2CDevHead = ptdev;
	}

	return ESUCCESS;
}

//在I2C设备链表中 找到名字是name的I2C设备
I2CDevice *I2CDevFind(char *name)
{
	if(NULLDEV == name)
	{
		return NULLDEV;
	}

	I2CDevice *tmp = I2CDevHead;
	while(tmp)
	{
		if( 0 == strcmp(tmp->name,name) )
		{
			return tmp;
		}
		
		tmp = tmp->next;
	}

	return NULLDEV;
}

//注册一个I2C设备链表
int I2CDevRegister(void)
{
	I2CDevice *tmp = I2CGetDevHead();
	if(NULLDEV == tmp)
	{
		return -EIO;
	}

	int num = I2CGetDevNum();
	for(unsigned int i=0; i<num ;i++)
	{
		if ( I2CDevInsert(&tmp[i]) != ESUCCESS )
		{
			return -EIO;
		}
	}
	return ESUCCESS;
}

