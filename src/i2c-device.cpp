#include <stdio.h>
#include <sys/ioctl.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <linux/i2c-dev.h>

#include "i2c-device.h"

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : I2CDevice
* 
* PURPOSE : Constructor for rd/wr to i2c devices
*
*-----------------------------------------------------------------------------*/
I2CDevice::I2CDevice(int i2c_bus)
{
    this->i2c_bus = i2c_bus;
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : I2CRead
* 
* PURPOSE : Read the i2c-devices register values into a buffer
*
*-----------------------------------------------------------------------------*/

int I2CDevice::I2CRead(char* filename)
{
    FILE* file;
    char i2cBuffer[I2C_MAX_BUF_SIZE];
    if ( (file = fopen(filename,"r")) == NULL )
    {  
        char* errorMsg;
        sprintf(errorMsg,"Fopen failed to open I2C device at path %s and returned error message %s \n",filename, strerror(errno));
        //Shakespeare::log(Priority::ERROR,s_cs1_subsystems[Hardware],errorMsg);
        //return CS1_FAILURE;
        return -1;
    }
    if ((fgets(i2cBuffer,I2C_MAX_BUF_SIZE,file))== NULL)
    {
        char* errorMsg;
        sprintf(errorMsg,"Fgets failed to read I2C device at path %s and returned error message %s \n",filename, strerror(errno));
        //Shakespeare::log(Priority::ERROR,s_cs1_subsystems[Hardware],errorMsg);
    }
    fclose(file);  
    printf("The value read is %s",i2cBuffer);
  
    return 1;
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : I2CWriteToRTC
* 
* PURPOSE : Write to the i2c-device register
*
*-----------------------------------------------------------------------------*/
int I2CDevice::I2CWriteToRTC(struct rtc_time rt)
{   
    char* fileHandler;
    sprintf(fileHandler,"/dev/rtc%d",i2c_bus);
    int file = open(fileHandler,O_RDWR);
    
    if (file < 0 )
    {
        char* errorMsg;
        sprintf(errorMsg,"Open failed to open RTC at %s and returned errno %s \n", fileHandler,strerror(errno));
        //Shakespeare::log(Priority:ERROR,s_cs1_subsystems[Hardware],errorMsg);
        return -1;
    }
    else
    {
        file = ioctl(file,RTC_SET_TIME,&rt);
        if ( file < 0 )
        {
            char* errorMsg;
            sprintf(errorMsg,"IOCTL failed to set RTC at %s and returned errno %s \n", fileHandler,strerror(errno));
        //Shakespeare::log(Priority:ERROR,s_cs1_subsystems[Hardware],errorMsg);

        }
        
    }
    close(file);            
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : I2CWrite
* 
* PURPOSE : Write to the i2c-device register
*
*-----------------------------------------------------------------------------*/
int I2CDevice::I2CWrite()
{
        


    

}
