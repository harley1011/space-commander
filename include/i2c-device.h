#ifndef I2C_DEVICE_H
#define I2C_DEVICE_H


class I2CDevice {
public:
    I2CDevice(int i2c_bus, int i2c_address, int buffer_size);
    int I2CRead();
    int I2CWrite();

private:
    int i2c_bus;
    int i2c_address;
    int buffer_size;
    char* reg_buffer;
};
#endif
