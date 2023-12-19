#include "devices.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "errno.h"
#include "mytype.h"
#include <string.h>
#include "cmsis_os.h"
#include "draw.h"
#include "resource.h"

#include <stdbool.h>
#include <stdlib.h>
#define STR_WIN			"WIN!"
#define STR_GAMEOVER	"GAMEOVER!"

#define UPT_MOVE_NONE	0
#define UPT_MOVE_RIGHT	1
#define UPT_MOVE_LEFT	2

#define BOARD_WIDTH		12
#define BOARD_HEIGHT	4
#define BLOCK_COLS		32
#define BLOCK_ROWS		5
#define BLOCK_COUNT		(BLOCK_COLS * BLOCK_ROWS)
#define SCREEN_WIDTH	128
#define SCREEN_HEIGHT	64

//一个砖块 宽度为3 长度为3
static const unsigned char block[] ={
	0x07,0x07,0x07,
};

static const unsigned char platform[] ={
	0x60,0x70,0x50,0x10,0x30,0xF0,0xF0,0x30,0x10,0x50,0x70,0x60,
};

static const unsigned char ballImg[] ={
	0x03,0x03,
};

static const unsigned char clearImg[] ={
	0,0,0,0,0,0,0,0,0,0,0,0,
};

typedef struct board_t{
	unsigned char x;
	unsigned char y;	
}board;

typedef struct ball_t{
	float x;				//位置
	float y;
	float velx;				//加速度
	float vely;
}ball;

QueueHandle_t gdirqueue = NULL;

static ball gameball = {
	.x = (SCREEN_WIDTH)/2,
	.y = (SCREEN_HEIGHT-8-2),
	.velx = 0.5,
	.vely = -0.6
};

static board gameboard = {
	.x = (SCREEN_WIDTH - BOARD_WIDTH)/2,
	.y = (SCREEN_HEIGHT-8)
};
	
static bool blocks[BLOCK_COUNT] = {0};		//是否显示该砖块
void DrawBolck()
{
	unsigned char index = 0;
	for(unsigned char x = 0 ;x<BLOCK_COLS;x++)
	{
		for(unsigned char y = 0 ;y<BLOCK_ROWS;y++)
		{	
			if(!blocks[index])
			{
				DrawBitMap(x*4, (y*4)+8, block, 3, 4);
				DrawFlushArea(x*4, (y*4)+8, 3, 8);
			}
		}
		index++;
	}
}

void MoveBoard(void *arg)
{
	DirData mydata = {0};
	//显示滑动板
	DrawBitMap(gameboard.x, gameboard.y, platform, 12, 8);
	DrawFlushArea(gameboard.x, gameboard.y, 12, 8);
	
	BaseType_t xStatus;
	unsigned char dir = 0;
	//循环读取按键值 如果是左右 则改变木板移动方向 否则 进入下一次循环
	while (1)
	{
		xStatus = xQueueReceive( gdirqueue, &mydata, portMAX_DELAY);
		if(pdPASS == xStatus)
		{
			dir = mydata.value;
		}else
		{
			dir = UPT_MOVE_NONE;
		}
		
        //隐藏滑动板
		DrawBitMap(gameboard.x, gameboard.y, clearImg, 12, 8);
		DrawFlushArea(gameboard.x, gameboard.y, 12, 8);
        
		if(UPT_MOVE_LEFT == dir)
		{
			gameboard.x -= 3;
		}
		else if(UPT_MOVE_RIGHT == dir)
		{
			gameboard.x += 3;
		}else
		{
			gameboard.x = gameboard.x;
		}

		
		//滑动板越界判断
		if(gameboard.x>240)
			gameboard.x = 0;
		if(gameboard.x>(128- BOARD_WIDTH))
			gameboard.x = SCREEN_WIDTH - BOARD_WIDTH;

		
		//显示滑动板
		DrawBitMap(gameboard.x, gameboard.y, platform, 12, 8);
		DrawFlushArea(gameboard.x, gameboard.y, 12, 8);

	}
}

static volatile unsigned char score = 0;
static volatile unsigned char lives = 3;
static volatile unsigned char first = 1;
void HibBlock()
{
    unsigned char buff[10];
    
    //画出砖块
	if(first)
	{
		first = 0;
		DrawBolck();
	}
	
	bool gameover =( (score>=160) || (255==lives) );
	
	//隐藏球
	DrawBitMap(gameball.x, gameball.y, clearImg, 2, 2);
	DrawFlushArea(gameball.x, gameball.y, 2, 8);

	//游戏没有结束 球运动
	if(!gameover)
	{
		gameball.x += gameball.velx;
		gameball.y += gameball.vely;
	
		//运动后判断 球是否碰撞砖块	
		unsigned char index = 0;
		for(unsigned char x = 0 ;x<BLOCK_COLS;x++)
		{
			for(unsigned char y = 0 ;y<BLOCK_ROWS;y++)
			{	
				//如果该砖块没有 被碰撞 并且小球的起始位于该砖块的位置之中 则发生碰撞
				if(!blocks[index] && gameball.x >= x * 4 && gameball.x < (x * 4) + 4 && gameball.y >= (y * 4) + 8 && gameball.y < (y * 4) + 8 + 4 )
				{
					blocks[index] = true;
					//隐藏该砖块
					DrawBitMap(x * 4, (y * 4) + 8, clearImg, 3, 4);
					DrawFlushArea(x * 4, (y * 4) + 8, 3, 8);
					score++;

					//改变球体的垂直加速度
					gameball.vely = gameball.vely*(-1);
				}
                index++;
			}
		}
	
		//运动后判断 球是否碰撞左右墙壁
		if(gameball.x < 0)
		{
			gameball.x = 0;
			gameball.velx = gameball.velx*(-1);
		}

		if((gameball.x+2) > SCREEN_WIDTH)
		{
			gameball.x = SCREEN_WIDTH - 2;
			gameball.velx = gameball.velx*(-1);
		}
	
		//运动后判断 球是否碰撞挡板
		if(gameball.x >= gameboard.x && gameball.x <= gameboard.x + BOARD_WIDTH && gameball.y+2 >= SCREEN_HEIGHT - BOARD_HEIGHT)
		{
			gameball.y = SCREEN_HEIGHT - BOARD_HEIGHT - 2;

			//改变球体的垂直加速度
			if(gameball.vely > 0)
				gameball.vely = gameball.vely*(-1);

			gameball.velx = ((float)rand() / (RAND_MAX / 2)) - 1; // -1.0 to 1.0
		}
	
		//运动后判断 球是否碰撞上下墙壁
		if(gameball.y < 0)
		{
			gameball.y = 0;
			gameball.vely = gameball.vely*(-1);
		}
		if((gameball.y+2) > SCREEN_HEIGHT)
		{
			gameball.y = SCREEN_HEIGHT - 2;
			gameball.vely = gameball.vely*(-1);
			lives--;
		}

		//显示球
		DrawBitMap(gameball.x, gameball.y, ballImg, 2, 2);
		DrawFlushArea(gameball.x, gameball.y, 2, 8);

		// Draw score
		sprintf(buff, "%u", score);
		DrawString(0, 0 , buff);

	    // Draw lives
	    if(lives != 255)
	    {
	        for(unsigned char i = 0;i<3;i++)
	        {
	            if (i < lives)
	                DrawBitMap((SCREEN_WIDTH - (3*8)) + (8*i), 0, livesImg, 7, 8);
	            else
	                DrawBitMap((SCREEN_WIDTH - (3*8)) + (8*i), 0, clearImg, 7, 8);
				
	            DrawFlushArea((SCREEN_WIDTH - (3*8)) + (8*i), 0, 7, 8);    
	        }
	    }   
	}

	// Got all blocks
	if(score >= BLOCK_COUNT)
	{
		sprintf(buff,STR_WIN);
		DrawString(50, 32, buff);
	}

	// No lives left (255 because overflow)
	if(lives == 255)
	{
		sprintf(buff, STR_GAMEOVER);
		DrawString(34, 32 , buff);
	}
}

void RotaryProcess(QueueHandle_t rotaryqueue)
{
	BaseType_t xstatus;
	RotartData mydata = {0};
	DirData dirdata = {0};
	int count = 0;
	int speed = 0;
	
	xstatus =  xQueueReceive(rotaryqueue,&mydata,0 );
	if(xstatus != pdPASS)
		return;
	speed = mydata.speed;
	dirdata.dev = 1;
	if(speed<0)
	{
		dirdata.value = UPT_MOVE_LEFT;
		speed = -speed;
	}else if(speed>0)
	{
		dirdata.value = UPT_MOVE_RIGHT;
	}else
    {
        dirdata.value = UPT_MOVE_NONE;
        return;
    }

	if(speed>100)
	{
		count = 4;
	}else if(speed>50)
	{
		count = 2;
	}else
	{
		count = 1;
	}

	for(unsigned char i= 0;i<count;i++)
		xQueueSend(gdirqueue, &dirdata, 0);
}

void IrdaProcess(QueueHandle_t irdaqueue)
{
	BaseType_t xstatus;
	IrdaData mydata = {0};
	DirData dirdata = {0};
	
	xstatus =  xQueueReceive(irdaqueue,&mydata,0);
	if(xstatus != pdPASS)
		return;
	
	dirdata.dev = 0;
	if(0x90 == mydata.data)
	{
		dirdata.value = UPT_MOVE_RIGHT;
	}else if(0xe0 == mydata.data)
	{
		dirdata.value = UPT_MOVE_LEFT;
	}else
    {
        dirdata.value = UPT_MOVE_NONE;
        return;
    }

	xQueueSend(gdirqueue, &dirdata, 0);
}

void MPU6050Process(QueueHandle_t mpuqueue)
{
	BaseType_t xstatus;
	MPUData mydata = {0};
	DirData dirdata = {0};
	
	xstatus =  xQueueReceive(mpuqueue,&mydata,0);
	if(xstatus != pdPASS)
		return;
	
	dirdata.dev = 0;
	if(mydata.angel > 91)
	{
		dirdata.value = UPT_MOVE_RIGHT;
	}else if(mydata.angel < 90)
	{
		dirdata.value = UPT_MOVE_LEFT;
	}else
    {
        dirdata.value = UPT_MOVE_NONE;
        return;
    }

	xQueueSend(gdirqueue, &dirdata, 0);
}

void MPUTask(void *arg)
{
    short AccX, AccY, AccZ, GyroX, GyroY, GyroZ;
	MPU6050Data angel = {0};
	MPUData mydata = {0};
	MPU6050Device *pmpu6050 = MPU6050DevGet();
	QueueHandle_t mpuqueue = GetMPU6050Queue();
	while(1)
	{
		pmpu6050->Read(pmpu6050,&AccX, &AccY, &AccZ, &GyroX, &GyroY, &GyroZ);

    	pmpu6050->Parse(pmpu6050,AccX, AccY, 0, 0, 0, 0,&angel);  
        mydata.dev = 0;
		mydata.angel = angel.angley;

		xQueueSend(mpuqueue, &mydata, 0);
        vTaskDelay(50);
	}
}

void InputTask(void *arg)
{
	ROTARYDevice * protary = GetROTARYDevice();
	if(NULLDEV == protary)
		;
	if(protary->Init(protary)!=ESUCCESS)
		;
    
    IDRADevice *irda = GetIRDADev();
	if(NULLDEV == irda)
	{
		;
	}
	if( irda->Init(irda)!= ESUCCESS )
        ;
	
	MPU6050Device *pmpu6050 = MPU6050DevGet();
	if(pmpu6050 == ESUCCESS)
		return;
    if(pmpu6050->Init(pmpu6050)!=ESUCCESS)
		return;
	QueueSetHandle_t xinputset = xQueueCreateSet(IRDAQUEUE+ROTARYQUEUE+MPUQUEUE);
	if(NULL == xinputset)
		;

	QueueHandle_t rotaryqueue = GetROTARYQueue();
	QueueHandle_t irdaqueue = GetIRDAQueue();
	QueueHandle_t mpuqueue = GetMPU6050Queue();
	xQueueAddToSet( rotaryqueue, xinputset);
	xQueueAddToSet( irdaqueue, xinputset);	
	xQueueAddToSet( mpuqueue, xinputset);	
	xQueueReset(mpuqueue);
	xQueueReset(irdaqueue);
	xQueueReset(rotaryqueue);
    xTaskCreate(MPUTask, "MPUTask", 128, NULL, osPriorityNormal, NULL);
	QueueSetMemberHandle_t QueueSetMember = NULL;
	while(1)
	{
		QueueSetMember =  xQueueSelectFromSet(xinputset,portMAX_DELAY);
		if(xinputset == NULL)
			continue;

		if(rotaryqueue == QueueSetMember)
		{
			RotaryProcess(rotaryqueue);
		}else if(irdaqueue == QueueSetMember)
		{
			IrdaProcess(irdaqueue);
		}else if(mpuqueue == QueueSetMember)
		{
			MPU6050Process(mpuqueue);
		}else
        {
            
        }
		
	}
}

void Game1Task(void *arg)
{
    
	DrawInit();
	Drawend();
	gdirqueue =  xQueueCreate(10, sizeof(DirData));
	if(NULL == gdirqueue)
	{
		;
	}
	
	xTaskCreate(InputTask, "InputTask", 128, NULL, osPriorityNormal, NULL);
	xTaskCreate(MoveBoard, "MoveBoard", 128, NULL, osPriorityNormal, NULL);
	while (1)
	{
		HibBlock();
		vTaskDelay(50);
	}
}
