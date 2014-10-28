#include <i2c-device.h>
#include <linux/rtc.h>

int main()
{
    I2CDevice i2cDevice(1);
    struct rtc_time rt = {
        10,
        10,
        10,
        10,
        10,
        105};

    i2cDevice.I2CWriteToRTC(rt);


}
