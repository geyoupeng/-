#ifndef __DRV_TIMER_H
#define __DRV_TIMER_H
#include "dev_timer.h"

TIMERDevice * TIMERGetDevHead(void);

//获取TIMER设备数量
int TIMERGetDevNum(void);

#endif /* __DRV_TIMER_H */

