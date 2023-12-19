#include "errno.h"
#include "mytype.h"
#include "drivers.h"
#include "string.h"

//GPIO设备链表头指针
static GPIODevice *GPIODevHead = NULLDEV;

//将GPIO设备节点插入 GPIO设备链表中
int GPIODevInsert(GPIODevice *ptdev)
{
	if(NULLDEV == ptdev)
	{
		return -EINVAL;
	}

	if(NULLDEV == GPIODevHead)
	{
		GPIODevHead = ptdev;
	}else
	{
		ptdev->next = GPIODevHead;
		GPIODevHead = ptdev;
	}

	return ESUCCESS;
}

//在GPIO设备链表中 找到名字是name的GPIO设备
GPIODevice *GPIODevFind(char *name)
{
	if(NULLDEV == name)
	{
		return NULLDEV;
	}

	GPIODevice *tmp = GPIODevHead;
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

//注册一个GPIO设备链表
int GPIODevRegister(void)
{
	GPIODevice *tmp = GPIOGetDevHead();
	if(NULLDEV == tmp)
	{
		return -EIO;
	}

	int num = GPIOGetDevNum();
	for(unsigned int i=0; i<num ;i++)
	{
		if ( GPIODevInsert(&tmp[i]) != ESUCCESS )
		{
			return -EIO;
		}
	}
	return ESUCCESS;
}

