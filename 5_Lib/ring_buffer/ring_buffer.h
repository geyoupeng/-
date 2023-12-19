#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H

//环形缓冲区信息对象
typedef struct RingBufferInfo{
	unsigned char *head;
	unsigned char *tail;
	unsigned char *validstart;
	unsigned char *validend;
	unsigned int validlength;
	unsigned int bufferlength;
}RingBufferInfo;

//环形缓冲区对象
typedef struct RingBuffer{
	RingBufferInfo info;
	int (*Write)(struct RingBuffer *ptdev,unsigned char *buf,unsigned int length);
	int (*Read)(struct RingBuffer *ptdev,unsigned char *buf,unsigned int length);
	int (*Clear)(struct RingBuffer *ptdev);
	int (*Free)(struct RingBuffer *ptdev);
}RingBuffer;

RingBuffer *NewRingBuffer(unsigned int length);
#endif /* __RING_BUFFER_H */

