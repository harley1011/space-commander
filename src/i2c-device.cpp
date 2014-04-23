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
* NAME : I2CRead
* 
* PURPOSE : Read the i2c-devices register values into a buffer
*
*-----------------------------------------------------------------------------*/

int I2CDevice::I2CRead(char* filename, char* i2c_buffer)
{
    FILE* file;
    if ( (file = fopen(filename,"r")) == NULL )
    {  
        char* error_msg;
        sprintf(error_msg,"Fopen failed to open I2C device at path %s and returned error message %s \n",filename, strerror(errno));
        //Shakespeare::log(Priority::ERROR,s_cs1_subsystems[Hardware],errorMsg);
        //return CS1_FAILURE;
        return -1;
    }
    if ((fgets(i2c_buffer,I2C_MAX_BUF_SIZE,file))== NULL)
    {
        char* error_msg;
        sprintf(error_msg,"Fgets failed to read I2C device at path %s and returned error message %s \n",filename, strerror(errno));
        //Shakespeare::log(Priority::ERROR,s_cs1_subsystems[Hardware],error_msg);
    }
    fclose(file);  
  
    return 1;
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : I2CWriteToRTC
* 
* PURPOSE : Write to the RTC to set the time
*
*-----------------------------------------------------------------------------*/
int I2CDevice::I2CWriteToRTC(struct rtc_time rt,int bus_number)
{   
    char* file_handler;
    sprintf(file_handler,"/dev/rtc%d",bus_number);
    int file = open(file_handler,O_RDWR);
    
    if (file < 0 )
    {
        char* error_msg;
        sprintf(error_msg,"Open failed to open RTC at %s and returned errno %s \n", file_handler,strerror(errno));
        //Shakespeare::log(Priority:ERROR,s_cs1_subsystems[Hardware],errorMsg);
        return -1;
    }
    else
    {
        file = ioctl(file,RTC_SET_TIME,&rt);
        if ( file < 0 )
        {
            char* error_msg;
            sprintf(error_msg,"IOCTL failed to set RTC at %s and returned errno %s \n", file_handler,strerror(errno));
        //Shakespeare::log(Priority:ERROR,s_cs1_subsystems[Hardware],error_msg);

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
