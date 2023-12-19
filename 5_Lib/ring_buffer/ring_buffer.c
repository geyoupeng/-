#include "ring_buffer/ring_buffer.h"
#include "errno.h"
#include <string.h>
#include <stdlib.h>
#include "stm32f1xx_hal.h"

static int RingBufferDrvWrite(struct RingBuffer *ptdev,unsigned char *buf,unsigned int length);
static int RingBufferDrvRead(struct RingBuffer *ptdev,unsigned char *buf,unsigned int length);
static int RingBufferDrvClear(struct RingBuffer *ptdev);
static int RingBufferDrvFree(struct RingBuffer *ptdev);

//在堆空间中创建一片length字节环形缓冲区
RingBuffer *NewRingBuffer(unsigned int length)
{
	if( 0==length || length>1024)
	{
		return NULL;
	}

	RingBuffer * buffer = NULL;;
	buffer = (RingBuffer *)malloc(sizeof(RingBuffer));
	if( NULL == buffer)
	{
		return NULL;
	}
	buffer->Write = RingBufferDrvWrite;
	buffer->Read = RingBufferDrvRead;
	buffer->Clear = RingBufferDrvClear;
	buffer->Free = RingBufferDrvFree;
	buffer->info.head = NULL;
	buffer->info.head = (unsigned char *)malloc(sizeof(unsigned char)*length);
	if( NULL == buffer->info.head )
	{
		free(buffer);
		return NULL;
	}
	buffer->info.tail = (unsigned char *)(buffer->info.head + length);
	buffer->info.validstart = buffer->info.head;
	buffer->info.validend = buffer->info.head;
	buffer->info.bufferlength = length;
	buffer->info.validlength = 0;
	return buffer;
}

//向缓冲区写入length字节的数据
static int RingBufferDrvWrite(struct RingBuffer *ptdev,unsigned char *buf,unsigned int length)
{
	//参数检错
	if(NULL == ptdev)
		return -EINVAL;
	if(NULL == buf)
		return -EINVAL;
	if(0 == length)
		return -EINVAL;

	unsigned int len = (unsigned int)(ptdev->info.tail - ptdev->info.validend);
	unsigned int len1 = 0;
	if(len > length)
	{
		memcpy(ptdev->info.validend,buf,length);
		ptdev->info.validend = ptdev->info.validend + length;
	}else
	{
		//要存储读取的数据 超过缓冲区尾部处理
		len1 = length - len;
		memcpy(ptdev->info.validend,buf,len);
		memcpy(ptdev->info.head,buf,len1);
		ptdev->info.validend = ptdev->info.head + len1;
	}
	
	ptdev->info.validlength = ptdev->info.validlength  + length;
	if(ptdev->info.validlength > ptdev->info.bufferlength)
	{
		//要存储的数据 超过缓冲区最大长度时 对旧数据进行覆盖
		ptdev->info.validlength = ptdev->info.bufferlength;
		ptdev->info.validstart = ptdev->info.validend;
	}
	return length;
}

//从缓冲区读取length字节的数据
static int RingBufferDrvRead(struct RingBuffer *ptdev,unsigned char *buf,unsigned int length)
{
	//参数检错
	if(NULL == ptdev)
		return -EINVAL;
	if(NULL == buf)
		return -EINVAL;
	if(0 == length)
		return -EINVAL;

	if(length > ptdev->info.validlength)
		length = ptdev->info.validlength;

	unsigned int len = (unsigned int)(ptdev->info.tail - ptdev->info.validstart);
	unsigned int len1 = 0;
	if(len > length)
	{
		memcpy(buf,ptdev->info.validstart,length);
		ptdev->info.validstart = ptdev->info.validstart + length;
	}else
	{
		//要读取的数据 超过缓冲区尾部处理
		len1 = length - len;
		memcpy(buf,ptdev->info.validstart,len);
		memcpy(buf+len,ptdev->info.head,len1);
		ptdev->info.validstart = ptdev->info.head + len1;
	}

	ptdev->info.validlength = ptdev->info.validlength  - length;
	return length;
}

//清除缓冲区的数据
static int RingBufferDrvClear(struct RingBuffer *ptdev)
{
	//参数检错
	if(NULL == ptdev)
		return -EINVAL;
	memset(ptdev->info.head,0,ptdev->info.bufferlength);
	ptdev->info.validstart = ptdev->info.head;
	ptdev->info.validend = ptdev->info.head;
	ptdev->info.validlength = 0;
	return ESUCCESS;
}

//释放缓冲区
static int RingBufferDrvFree(struct RingBuffer *ptdev)
{
	//参数检错
	if(NULL == ptdev)
		return -EINVAL;

	memset(ptdev->info.head,0,ptdev->info.bufferlength);
	free(ptdev->info.head);
	memset(ptdev,0,sizeof(struct RingBuffer));
	free(ptdev);

	return ESUCCESS;
}

