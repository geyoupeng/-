#include "errno.h"
#include "mytype.h"
#include "drivers.h"
#include "string.h"
#include "devices.h"

//TIMER设备链表头指针
static TIMERDevice *TIMERDevHead = NULLDEV;

//将TIMER设备节点插入 TIMER设备链表中
int TIMERDevInsert(TIMERDevice *ptdev)
{
	if(NULLDEV == ptdev)
	{
		return -EINVAL;
	}

	if(NULLDEV == TIMERDevHead)
	{
		TIMERDevHead = ptdev;
	}else
	{
		ptdev->next = TIMERDevHead;
		TIMERDevHead = ptdev;
	}

	return ESUCCESS;
}

//在TIMER设备链表中 找到名字是name的TIMER设备
TIMERDevice *TIMERDevFind(char *name)
{
	if(NULLDEV == name)
	{
		return NULLDEV;
	}

	TIMERDevice *tmp = TIMERDevHead;
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

//注册一个TIMER设备链表
int TIMERDevRegister(void)
{
	TIMERDevice *tmp = TIMERGetDevHead();
	if(NULLDEV == tmp)
	{
		return -EIO;
	}

	int num = TIMERGetDevNum();
	for(unsigned int i=0; i<num ;i++)
	{
		if ( TIMERDevInsert(&tmp[i]) != ESUCCESS )
		{
			return -EIO;
		}
	}
	return ESUCCESS;
}


