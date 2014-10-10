#include <stdio.h>

class I2CDevice {
    public:
        int I2CBus;
        int I2CAddress;

        I2CDevice(int bus, int address);
        int I2CRead();
        int I2CWrite();



}
