#include "errno.h"
#include "mytype.h"
#include "drivers.h"
#include "string.h"
#include "devices.h"

//PWM设备链表头指针
static PWMDevice *PWMDevHead = NULLDEV;

//将PWM设备节点插入 PWM设备链表中
int PWMDevInsert(PWMDevice *ptdev)
{
	if(NULLDEV == ptdev)
	{
		return -EINVAL;
	}

	if(NULLDEV == PWMDevHead)
	{
		PWMDevHead = ptdev;
	}else
	{
		ptdev->next = PWMDevHead;
		PWMDevHead = ptdev;
	}

	return ESUCCESS;
}

//在PWM设备链表中 找到名字是name的PWM设备
PWMDevice *PWMDevFind(char *name)
{
	if(NULLDEV == name)
	{
		return NULLDEV;
	}

	PWMDevice *tmp = PWMDevHead;
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

//注册一个PWM设备链表
int PWMDevRegister(void)
{
	PWMDevice *tmp = PWMGetDevHead();
	if(NULLDEV == tmp)
	{
		return -EIO;
	}

	int num = PWMGetDevNum();
	for(unsigned int i=0; i<num ;i++)
	{
		if ( PWMDevInsert(&tmp[i]) != ESUCCESS )
		{
			return -EIO;
		}
	}
	return ESUCCESS;
}


