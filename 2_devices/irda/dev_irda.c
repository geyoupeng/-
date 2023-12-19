#include "errno.h"
#include "devices.h"
#include "libs.h"
#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "cmsis_os.h"
#include "mytype.h"
#include "string.h"

static int IRDADrvInit(struct IDRADev *ptdev);
static int IRDADrvRaed(struct IDRADev *ptdev);

#if 0
/* 20键遥控器码表 */
static const unsigned char KeyCode[20] = {
0x45, 0x47, 0x44, 0x40, 0x43, 
0x07, 0x15, 0x09, 0x16, 0x19, 
0x0D, 0x0C, 0x18, 0x5E, 0x08, 
0x1C, 0x5A, 0x42, 0x52, 0x4A
};
static char KeyName[20][10] = {
"Open",   "Menu",    "Test",    "+", "Return", 
"Back",   "Suspend", "Forward", "0", "-", 
"Cancle", "1",       "2",       "3", "4", 
"5",      "6",       "7",       "8", "9"
};
#endif


const unsigned char KeyCode[]= {0xa2, 0xe2, 0x22, 0x02, 0xc2, 0xe0, 0xa8, 0x90, \
                        0x68, 0x98, 0xb0, 0x30, 0x18, 0x7a, 0x10, 0x38, \
                        0x5a, 0x42, 0x4a, 0x52, 0x00};
static char KeyName[20][10]= {"Power", "Menu", "Test", "+", "Return", "Left", "Play", "Right",
                        "0", "-", "C", "1", "2", "3", "4", "5", 
                        "6", "7", "8", "9"};

static struct IDRADev gIRDADev = {
	.name = "NEC IRDA",
	.key = NULL,
	.value = 0,
	.Init = IRDADrvInit,
	.Raed = IRDADrvRaed	
};
static GPIODevice *pb1 = NULL;
static TIMERDevice *ptimer1 = NULL;
static RingBuffer *pbuffer = NULL;
static QueueHandle_t girdaqueue = NULL;

static unsigned int g_IRReceiverIRQ_Timers[68] = {0};
static int g_IRReceiverIRQ_Cnt = 0;

static int code = 0;	//记录上一个 红外射线的值

static int isRepeatedKey(void)
{
	unsigned int time;

	/* 1. 判断重复码 : 9ms的低脉冲, 2.25ms高脉冲  */
	time = (g_IRReceiverIRQ_Timers[1] - g_IRReceiverIRQ_Timers[0])*10;

	if(time>9500 || time<8500)
	{
		return 0;
	}

	time = (g_IRReceiverIRQ_Timers[2] - g_IRReceiverIRQ_Timers[1])*10;
	if(time>2750 || time<1750)
	{
		return 0;
	}	

	return 1;
}

static volatile unsigned int count = 0;

static int IRReceiver_IRQTimes_Parse(void)
{
	
	unsigned long long time;
	int i;
	int m, n;
	unsigned char datas[4];
	unsigned char data = 0;
	int bits = 0;
	int byte = 0;
	IrdaData irdata = {0};
	
	/* 1. 判断前导码 : 9ms的低脉冲, 4.5ms高脉冲  */
	time = (g_IRReceiverIRQ_Timers[1] - g_IRReceiverIRQ_Timers[0])*10;
	if (time < 8500 || time > 9500)
	{
		return -1;
	}

	time = (g_IRReceiverIRQ_Timers[2] - g_IRReceiverIRQ_Timers[1])*10;
	if (time < 3500 || time > 5500)
	{
		return -1;
	}

	/* 2. 解析数据 */
	for (i = 0; i < 32; i++)
	{
		m = 3 + i*2;
		n = m+1;
		time = (g_IRReceiverIRQ_Timers[n] - g_IRReceiverIRQ_Timers[m])*10;
		data <<= 1;
		bits++;
		if (time > 1000)
		{
			/* 得到了数据1 */
			data |= 1;
		}

		if (bits == 8)
		{
			datas[byte] = data;
			byte++;
			data = 0;
			bits = 0;
		}
	}

	/* 判断数据正误 */
	datas[1] = ~datas[1];
	datas[3] = ~datas[3];
	
	if ((datas[0] != datas[1]) || (datas[2] != datas[3]))
	{
        g_IRReceiverIRQ_Cnt = 0;
        return -1;
	}

	irdata.dev = 0;
	irdata.data = datas[2];		
	xQueueSendFromISR(girdaqueue, &irdata, 0);
	code = datas[2];
	gIRDADev.value = datas[2];
    return 0;
}

static void IRDAIrqHandler(void)
{
	if(ptimer1 != NULL)
		count = ptimer1->GetCount(ptimer1);
	//if(pbuffer != NULL)
		//pbuffer->Write(pbuffer,(unsigned char *)&count,sizeof(unsigned int));

	g_IRReceiverIRQ_Timers[g_IRReceiverIRQ_Cnt] = count;

	/* 2. 累计中断次数 */
	g_IRReceiverIRQ_Cnt++;

	/* 3. 次数达标后, 解析数据, 放入buffer */
	if (g_IRReceiverIRQ_Cnt == 4)
	{
		/* 是否重复码 */
		if (isRepeatedKey())
		{
			/* device: 0, val: 0, 表示重复码 */
			gIRDADev.value = code;
            IrdaData irdata = {0};
            irdata.dev = 0;
            irdata.data = code;		
            xQueueSendFromISR(girdaqueue, &irdata, 0);
			g_IRReceiverIRQ_Cnt = 0;
		}
	}
	if (g_IRReceiverIRQ_Cnt == 68)
	{
		IRReceiver_IRQTimes_Parse();
		g_IRReceiverIRQ_Cnt = 0;
	}
}

//获得红外遥控数据队列
QueueHandle_t GetIRDAQueue(void)
{
	return girdaqueue;
}

//获得红外遥控设备对象
IDRADevice *GetIRDADev(void)
{
	return &gIRDADev;
}

static int IRDADrvInit(struct IDRADev *ptdev)
{
	//初始化GPIOB_1 红外接收引脚
	pb1 =  GPIODevFind("IRDA");
	if(NULL == pb1)
	{
		return -EIO;
	}
	pb1->IrqHandler = IRDAIrqHandler;
	if( pb1->Init(pb1)!=ESUCCESS )
	{
		return -EIO;
	}

	ptimer1 = TIMERDevFind("TIMER1");
	if(NULL == ptimer1)
	{
		return -EIO;
	}
	if( ptimer1->Init(ptimer1)!=ESUCCESS )
	{
		return -EIO;
	}
	if( ptimer1->Control(ptimer1,SET_PERIOD_VALUE,10000)!=ESUCCESS )
	{
		return -EIO;
	}
	if( ptimer1->Start(ptimer1)!=ESUCCESS )
	{
		return -EIO;
	}

	pbuffer = NewRingBuffer(256);
	if(pbuffer == NULL)
		return -EIO;

	girdaqueue = xQueueCreate(10, sizeof(IrdaData));
	return ESUCCESS;
}

static int IRDADrvEncode(struct IDRADev *ptdev)
{
	if(NULL == ptdev)
	{
		return -EINVAL;
	}
	unsigned char PQ = 0;

	unsigned int duty = 0;
	unsigned int tick[2] = {0};
	unsigned int timeout = 1000;
	unsigned int step = 0;

	unsigned int buffer[64] = {0};
	unsigned int i = 0;
	unsigned int cnt = 0;
    int count = 0;
	while(timeout)
	{
        timeout--;
		if( pbuffer->Read(pbuffer,(unsigned char *)&tick[PQ],4)!= 4)
		{
			osDelay(1);
			continue;
		}
		PQ++;
		if(2 == PQ)
		{
			duty = (tick[1] - tick[0])*10;
			tick[0] = tick[1];
			PQ = 1;
		}

		switch(step)
		{
			case 0:
				if(duty<=9500 && duty>=8500)
				{
					step++;
				}
				break;
			case 1:
				if(duty<=5000 && duty>=4000)
				{
					//引导码
					step++;
				}else if(duty<=2750 && duty>=1750)
				{
					//重复码
					step = 0;
                    return code;
				}else
				{
					step = 0;
				}
				break;
			case 2:
				timeout = 1000;
				buffer[i] = duty;
				i++;
				if(i == 64)
				{
					step++;
					i = 0;
				}
				break;
			case 3:
				for(unsigned int n = 0; n<64; n+=2)
				{
					if(buffer[n]>=410 && buffer[n]<=550)
					{
						if(buffer[n+1]>=1290 && buffer[n+1]<=2290)
							count = (1<<cnt) + count;
						else if(buffer[n+1]>=500 && buffer[n+1]<=650)
                            ;
					}
					cnt++;
				}
				step++;
				break;
			case 4:
				timeout = 1000;
				code = (count>>16)&(0xFF);
				step = 0;
				PQ = 0;
				return code;
		}
	}

	return -EIO;
}	

static int IRDADrvRaed(struct IDRADev *ptdev)
{
	if(NULL == ptdev)
	{
		return -EINVAL;
	}
	
	if(gIRDADev.value == 0)
		return -EIO;

	for(unsigned int i=0;i<20;i++)
	{
		if(KeyCode[i] == gIRDADev.value)
		{
			ptdev->key = &(KeyName[i][0]);
			gIRDADev.value = 0;
			return ESUCCESS;
		}
	}
	return -EIO;
}

