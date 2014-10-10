#include <stdio.h>
#include <sys/ioctl.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "i2c-device.h"

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : I2CDevice
* 
* PURPOSE : Constructor
*
*-----------------------------------------------------------------------------*/
I2CDevice::I2CDevice(int i2c_bus, int i2c_address, int buffer_size)
{
    this->i2c_bus = i2c_bus;
    this->i2c_address = i2c_address;
    this->buffer_size = buffer_size;
    reg_buffer = new char[buffer_size];
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : I2CRead
* 
* PURPOSE : Read the i2c-devices register values into a buffer
*
*-----------------------------------------------------------------------------*/

int I2CDevice::I2CRead()
{
    char* filename;
    sprintf(filename,"/dev/i2c-%d",i2c_bus);
    
    int file = open(filename,O_RDWR);
    
    if (file < 0 )
    {
        fprintf(stderr,"Error failed to open i2c-device on %s",filename);
        return -1;       
    }
    
    if (ioctl(file,I2C_SLAVE,i2c_address) < 0)
    {
        fprintf(stderr,"Error failed to open i2c slave address %d", i2c_address);
        return -1;
    }

    if (read(file,reg_buffer,buffer_size) != 2)
    {   
        fprint(stderr,"Error failed to read from i2c-device on %s with address %s", filename,i2c_address); 
        return -1
    }
    return 1;
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
    return 1;


}
