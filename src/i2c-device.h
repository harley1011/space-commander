#ifndef I2C_DEVICE_H
#define I2C_DEVICE_H

#include <linux/rtc.h>

#define I2C_MAX_BUF_SIZE 50
class I2CDevice {
public:
    I2CDevice(int i2c_bus);
    int I2CRead(char* filename);
    int I2CWrite();
    int I2CWriteToRTC(struct rtc_time rt); 
private:
    bool rtc_device;
    int i2c_bus;
    char* filename;
    char* reg_buffer;
};
#endif
