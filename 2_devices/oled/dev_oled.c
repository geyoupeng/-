#include "devices.h"
#include "mytype.h"
#include "errno.h"
#include "oled_font.h"

static int OLEDInit(struct OLEDDev *ptdev);
static int OLEDDisplayOn(struct OLEDDev *ptdev);
static int OLEDDisplayOff(struct OLEDDev *ptdev);
static int OLEDShowString(struct OLEDDev *ptdev,unsigned char x,unsigned char y,unsigned char *str);
static int OLEDClear(struct OLEDDev *ptdev);
static int  OLEDFlushRegion(struct OLEDDev *ptdev,unsigned char col, unsigned char row, unsigned char width, unsigned char heigth);
static unsigned char oledBuffer[FRAME_WIDTH * 8] = {0};
static GPIODevice *pscl = NULLDEV;
static GPIODevice *psda = NULLDEV;

static I2CDevice *pi2c = NULLDEV;
static OLEDDevice gOLEDDev = {
	.name = "oled",
	.Init = OLEDInit,
	.DisplayOn = OLEDDisplayOn,
	.DisplayOff = OLEDDisplayOff,
	.ShowString = OLEDShowString,
	.Clear = OLEDClear,
	.FlushRegion = OLEDFlushRegion
};

OLEDDevice *OLEDDevGet()
{
	return &gOLEDDev;
}

unsigned char *OLEDBufferGet()
{
	return oledBuffer;
}

static int OLEDWriteByte(unsigned char data,unsigned char flag)
{

	unsigned char reg = 0;
	if(flag)
	{
		reg =0x00;
	}else
	{
		reg =0x40;
	}
    
    pi2c->Write(pi2c,&data,1,reg);
	return ESUCCESS;
}

static int OLEDWriteNByte(unsigned char *data,unsigned short length)
{
	unsigned char reg = 0x40;
    int ret = pi2c->Write(pi2c,data,length,reg);
	if(ret != length)
		return -EIO;
	return ESUCCESS;
}

static int SetPageAddr(unsigned char page)
{
	if(page > 8)
		return -EINVAL;
	
	int ret = OLEDWriteByte(0xB0|page,1);			//设置行
	if(ret != ESUCCESS)
	{
		return -EIO;
	}

	return ESUCCESS;
}

static int SetColAddr(unsigned char col)
{
	if(col > 128)
		return -EINVAL;
	int ret = OLEDWriteByte(0x00|(col&0x0F),1);			//设置列的低位
	if(ret != ESUCCESS)
	{
		return -EIO;
	}

	ret = OLEDWriteByte(0x10|((col&0xF0)>>4),1);	//设置列的高位
	if(ret != ESUCCESS)
	{
		return -EIO;
	}

	return ESUCCESS;
}

static int OLEDSetPosition(unsigned char page,unsigned char col)
{
	int ret = SetPageAddr(page);
	if(ret != ESUCCESS)
	{
		return -EIO;
	}
	ret = SetColAddr(col);
	if(ret != ESUCCESS)
	{
		return -EIO;
	}
	return ESUCCESS;
}

static int  OLEDFlushRegion(struct OLEDDev *ptdev,unsigned char col, unsigned char row, unsigned char width, unsigned char heigth)
{
    unsigned char i;
    unsigned char page = row / 8;
    unsigned char page_cnt = ((row % 8) + heigth +7)/8;
    int ret = -1;
    for (i = 0; i < page_cnt; i++)
    {
        if (page+i < 8)
        {
           ret = OLEDSetPosition(page+i, col);
		   if(ret != ESUCCESS)
		   {
				return -EIO;
		   }
           ret = OLEDWriteNByte(&oledBuffer[col + (page + i) * FRAME_WIDTH], width);
		   if(ret != ESUCCESS)
		   {
				return -EIO;
		   }
        }
   }
	return ESUCCESS;
}

static int OLEDDisplayOn(struct OLEDDev *ptdev)
{
	if(NULLDEV == ptdev)
	{
		return -EINVAL;
	}

	int ret = OLEDWriteByte(0xAF,1);	//打开显示
	if(ret != ESUCCESS)
	{
		return -EIO;
	}
	return ESUCCESS;
}

static int OLEDDisplayOff(struct OLEDDev *ptdev)
{
	if(NULLDEV == ptdev)
	{
		return -EINVAL;
	}

	int ret = OLEDWriteByte(0xAE,1);	//关闭显示
	if(ret != ESUCCESS)
	{
		return -EIO;
	}
	return ESUCCESS;
}

static int OLEDSetCursor(unsigned char line,unsigned char col)
{
	if(line >= 8)
		return -EINVAL;
	if(col >= 128)
		return -EINVAL;
	
	int ret = OLEDWriteByte(0xB0|line,1);	//设置行
	if(ret != ESUCCESS)
	{
		return -EIO;
	}

	ret = OLEDWriteByte(0x00|(col&0x0F),1);	//设置列
	if(ret != ESUCCESS)
	{
		return -EIO;
	}

	ret = OLEDWriteByte(0x10|((col&0xF0)>>4),1);	//设置列
	if(ret != ESUCCESS)
	{
		return -EIO;
	}
	return ESUCCESS;
}


static int OLEDClear(struct OLEDDev *ptdev)
{
    unsigned char data[128] = {0};
	for(unsigned char i=0;i<8;i++)
	{
		OLEDSetCursor(i,0);
        //OLEDWriteNByte(data,128);
		for(unsigned char j=0;j<128;j++)
			OLEDWriteByte(0x00,0);
	}
    return ESUCCESS;
}

static int OLEDShowChar(unsigned char line,unsigned char col,unsigned char Char)
{
	unsigned char i = 0;
	OLEDSetCursor((line-1)*2,(col-1)*8);
	for(i = 0; i < 8; i++)
	{
		OLEDWriteByte(OLED_F8x16[Char - ' '][i],0);
	}
	OLEDSetCursor((line-1)*2+1,(col-1)*8);
	for(i = 0; i < 8; i++)
	{
		OLEDWriteByte(OLED_F8x16[Char - ' '][i+8],0);
	}
	return ESUCCESS;
}

static int OLEDShowString(struct OLEDDev *ptdev,unsigned char line,unsigned char col,unsigned char *str)
{
	unsigned char ch = *str;
	while (ch != '\0')
	{
		int ret = OLEDShowChar(line,col,ch);
		if(ret != ESUCCESS)
			return -EIO;
		col++;
		if(col > 16)
		{
			col = 1;
			line = line + 1;
			if(line > 4)
				line = 1;
		}
		str++;
		ch = *str;
	}
	return ESUCCESS;
}

static int OLEDInit(struct OLEDDev *ptdev)
{
	if(NULLDEV == ptdev)
	{
		return -EINVAL;
	}

	pscl = GPIODevFind("PB8");
	if(NULLDEV == pscl)
	{
		;
	}
	pscl->Init(pscl);
	psda = GPIODevFind("PB9");
	if(NULLDEV == psda)
	{
		;
	}
	psda->Init(psda);
    
    pi2c = I2CDevFind("I2C1");
	if(NULLDEV == pi2c)
	{
		;
	}
	pi2c->Init(pi2c);
    pi2c->slave_addr = 0x78;
	
	OLEDWriteByte(0xAE,1);	    //关闭显示

	OLEDWriteByte(0xD5,1);	    //设置显示时钟分频比/振荡器频率
	OLEDWriteByte(0x80,1);
	
	OLEDWriteByte(0xA8,1);	    //设置多路复用率和分辨率
	OLEDWriteByte(0x3F,1);
	
	OLEDWriteByte(0xD3,1);	     //设置显示偏移
	OLEDWriteByte(0x00,1);
	
	OLEDWriteByte(0x40,1);		//设置显示开始行
	
	OLEDWriteByte(0xA1,1);		//设置左右方向，0xA1正常 0xA0左右反置
	
	OLEDWriteByte(0xC8,1);		//设置上下方向，0xC8正常 0xC0上下反置
	
	OLEDWriteByte(0xDA,1);		//设置COM引脚硬件配置
	OLEDWriteByte(0x12,1);

	OLEDWriteByte(0x81,1);		//设置对比度控制
	OLEDWriteByte(0xCF,1);
	
	OLEDWriteByte(0xD9,1);		//设置预充电周期
	OLEDWriteByte(0xF1,1);

	OLEDWriteByte(0xDB,1);		//设置VCOMH取消选择级别
	OLEDWriteByte(0x30,1);

	OLEDWriteByte(0xA4,1);		//设置整个显示打开/关闭

	OLEDWriteByte(0xA6,1);		//设置正常/倒转显示

	OLEDWriteByte(0x8D,1);		//设置充电泵
	OLEDWriteByte(0x14,1);

	OLEDWriteByte(0xAF,1);		//开启显示
    
	OLEDClear(ptdev);				//OLED清屏
    
	return ESUCCESS;
}

