#include "devices.h"
#include "mytype.h"
#include "FreeRTOS.h"
#include "task.h"
#include "errno.h"
#include "cmsis_os.h"
#include <string.h>
#include <stdio.h>


static OLEDDevice *poled=NULL;
void MPU6050Task(void *argument)
{
	short AccX, AccY, AccZ, GyroX, GyroY, GyroZ;

	MPU6050Data angel = {0};
	MPU6050Device *pmpu6050 = MPU6050DevGet();
	if(pmpu6050 == ESUCCESS)
		return;
    if(pmpu6050->Init(pmpu6050)!=ESUCCESS)
		return;

	char buf[16] = {0};
    while (1)
    {    
        pmpu6050->Read(pmpu6050,&AccX, &AccY, &AccZ, &GyroX, &GyroY, &GyroZ);

		sprintf(buf,"X: %d %d",AccX,GyroX);
        poled->Clear(poled);
		poled->ShowString(poled,1, 1,buf);
        memset(buf,0,16);   

        pmpu6050->Parse(pmpu6050,AccX, AccY, 0, 0, 0, 0,&angel);  
		sprintf(buf,"Angel: %d %d",angel.anglex,angel.angley);
        //poled->Clear(poled);
		poled->ShowString(poled,2, 1,buf);
        memset(buf,0,16);
		osDelay(100);
	}
}

void mpu6050_test()
{
	int ret = GPIODevRegister();
	if(ret != ESUCCESS)
		return;

	ret = I2CDevRegister();
	if(ret != ESUCCESS)
		return;
	poled =  OLEDDevGet();
	if(NULLDEV == poled)
	{
		;
	}
    
    osDelay(1000);
    if( poled->Init(poled)!= ESUCCESS )
        ;
	//poled->ShowString(poled,1,1,"hello");
    xTaskCreate(MPU6050Task,"MPU6050Task",128,NULL,osPriorityNormal,NULL);

}
