#ifndef INA219_I2C_H
#define INA219_I2C_H

#include <sys/types.h>

#ifdef __cplusplus
"C" {
#endif

#define I2C_BUS "/dev/i2c-1"
#define INA_ADDR 0x40
#define SHUNT_RESISTOR 0.12

void init_ina_i2c();
u_int16_t i2c_smbus_read_byte_data(u_int8_t command);
float readCurrent();

#ifdef __cplusplus
}
#endif

#endif /* INA219_I2C_H */