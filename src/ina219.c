//
// Created by Daniel Vilas Perulan on 9/12/17.
//
#include "ina219.h"
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

int file;

int i2c_smbus_access(char read_write, u_int8_t command,
                                     int size, union i2c_smbus_data *data)
{
    struct i2c_smbus_ioctl_data args;

    args.read_write = read_write;
    args.command = command;
    args.size = size;
    args.data = data;
    return ioctl(file,I2C_SMBUS,&args);
}


u_int16_t i2c_smbus_read_byte_data(u_int8_t command)
{
    union i2c_smbus_data data;
    if (i2c_smbus_access(I2C_SMBUS_READ,command,
                         I2C_SMBUS_WORD_DATA,&data))
        return -1;
    else{
        u_int16_t ret = 0x0000;
        ret = (0x00FF & data.word)<<8;
        ret +=  (0xFF00 & data.word)>> 8;
        return ret;
    }
}

float  readCurrent(){
    int sv = i2c_smbus_read_byte_data(0x01);
    return ((float)sv)/(100.0*SHUNT_RESISTOR);
}

void init_ina_i2c(){


    printf("Ina219 I2C init\n");

    if ((file = open(I2C_BUS, O_RDWR)) < 0) {
        /* ERROR HANDLING: you can check errno to see what went wrong */
        perror("Failed to open the i2c bus");
        exit(1);
    }
    if (ioctl(file, I2C_SLAVE, INA_ADDR) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        exit(1);
    }
}
