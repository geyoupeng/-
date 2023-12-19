#include "devices.h"
#include "smallfont.h"
#include "errno.h"
#include "FreeRTOS.h"
#include "task.h"
#include "draw.h"

static OLEDDevice * poled = NULL;
static unsigned char *oledBuffer;
static unsigned char ReadPixel(const unsigned char * addr)
{
	return *addr;
}

//将newdata 从开始位将 num个位的数据 更新到olddata中
static unsigned char SetBits(unsigned char olddata,unsigned char newdata,unsigned char startbit,unsigned char num,unsigned char oldstartbit)
{
    
	unsigned char mask1 = (1<<num) - 1;
    unsigned char mask2 = (1<<num) - 1;
    mask1 = mask1<<startbit;
    newdata = newdata&mask1;
    newdata = newdata>>startbit;
    newdata = newdata<<oldstartbit;
    
    mask2 = mask2<<oldstartbit;
    olddata = olddata&(~mask2);

    
	olddata = olddata|newdata;
	
	return olddata;
}

//
void DrawBitMap(unsigned char x, unsigned char  y, const unsigned char * bitmap, unsigned char  width, unsigned char  heigth)
{

	unsigned char page_start = y / 8;

	//第一页需要改变的位数为 8 - offset	最后一页需要改变的位为offset
	unsigned char offset = y % 8;							//像素的偏差值

	unsigned char size = 0;

	unsigned char numbit = 8 - offset;
	unsigned char i = 0;
	
	while(heigth)
	{
		if(heigth > 8)
		{
			size = 8;
		}else
		{
			size = heigth;
		}
		const unsigned char* bitbase = bitmap + (i*width);		            //bitmap基地址		页增加1 bitbase+width
		for(unsigned int j= 0;j<width;j++)
		{	
			if(!offset)											            //没有偏差 写数据的地方为 8的倍数 在整行的位置	
			{
				unsigned int oledaddr = x + j + ((page_start+i)*128);		//oledaddr基地址	 页增加1 
				// Read pixels
				unsigned char pixel = ReadPixel(bitbase+j);		            //获取要写入的像素点值 
				
				/* 把pixels中的bits位设置进oledBuffer */
				oledBuffer[oledaddr] = SetBits(oledBuffer[oledaddr], pixel, 0, size,0);
			}else
			{
				unsigned char page1= (size+y+8*i)/8;
				unsigned char page2= (y+8*i)/8+1;
				unsigned int oledaddr1 = x + j + ((page_start+i)*128);			//oledaddr基地址1	 页增加1 
				// Read pixel
				unsigned char pixel = ReadPixel(bitbase+j);		//获取要写入的像素点值 
				if(page2 > page1)
				{
					/* 把pixel中的bits位设置进oledBuffer */
					oledBuffer[oledaddr1] = SetBits(oledBuffer[oledaddr1], pixel, 0, size,offset);
				}else
				{	
					unsigned int oledaddr2 = x + j + ((page_start+i+1)*128);
					//oledaddr基地址1	 页增加1 
					/* 把pixel中的bits位设置进oledBuffer */
					oledBuffer[oledaddr1] = SetBits(oledBuffer[oledaddr1], pixel, 0, numbit,offset);

					/* 把pixel中的bits位设置进oledBuffer */
					oledBuffer[oledaddr2] = SetBits(oledBuffer[oledaddr2], pixel, numbit, size - numbit,0);
				}	
			}
			
		}
		i++;
		heigth = heigth - size;
	}
	#if 0
	for(unsigned int i= 0;i<page_count;i++)
	{
			const unsigned char* bitbase = bitmap + (i*width);		//bitmap基地址		页增加1 bitbase+width

			unsigned char numbit = 8 - offset;

			if(heigth > 8)
			{
				size = 8;
			}else
			{
				size = heigth;
			}
			
			for(unsigned int j= 0;j<width;j++)
			{	if(!offset)											//没有偏差 写数据的地方为 8的倍数 在整行的位置	
				{
					unsigned int oledaddr = x + j + ((page_start+i)*128);		//oledaddr基地址	 页增加1 
					// Read pixels
					unsigned char pixel = ReadPixel(bitbase+j);		//获取要写入的像素点值 
					
					/* 把pixels中的bits位设置进oledBuffer */
					oledBuffer[oledaddr] = SetBits(oledBuffer[oledaddr], pixel, 0, numbit);
				}else
				{
					
					unsigned int oledaddr1 = x + j + ((page_start+i)*128);			//oledaddr基地址1	 页增加1 
					unsigned int oledaddr2 = x + j + ((page_start+i+1)*128);		//oledaddr基地址1	 页增加1 
					// Read pixel
					unsigned char pixel = ReadPixel(bitbase+j);		//获取要写入的像素点值 
					
					/* 把pixel中的bits位设置进oledBuffer */
					oledBuffer[oledaddr1] = SetBits(oledBuffer[oledaddr1], pixel, 0, numbit);

					/* 把pixel中的bits位设置进oledBuffer */
					oledBuffer[oledaddr2] = SetBits(oledBuffer[oledaddr2], pixel, numbit, offset);
				}
				
			}
	
	}
	#endif

}

void DrawFlushArea(unsigned char x, unsigned char y, unsigned char width, unsigned char heigth)
{
	if(poled == NULL)
		return;
    static volatile int OLEDInUsed = 0;
    while (OLEDInUsed);
    OLEDInUsed = 1;
	poled->FlushRegion(poled,x, y, width, heigth);
    OLEDInUsed = 0;
}

extern unsigned char smallFont[][5];
void DrawString(unsigned char x, unsigned char y,char *string)
{
	unsigned char count = 0;
    while (*string)
	{
		unsigned char col = x+count*7;
		char ch = *string  - 0x20;
		
		DrawBitMap(col, y, smallFont[(unsigned char)ch], SMALLFONT_WIDTH,SMALLFONT_HEIGHT);
		DrawFlushArea(col, y, SMALLFONT_WIDTH, SMALLFONT_HEIGHT);
		string++;
		count++;
	}

}

void Drawend()
{
	poled->Clear(poled); //刷新屏幕的意思, OLED的刷新需要100ms, 太慢了    
}

void DrawInit()
{
	poled= OLEDDevGet();
	if(poled == NULL)
		return;
    osDelay(1000);
	if(poled->Init(poled)!=ESUCCESS)
		return;
	
	oledBuffer = OLEDBufferGet();
}
