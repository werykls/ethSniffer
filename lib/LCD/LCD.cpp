#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <thread>
#include <chrono>
#include <string>

#include "../structures.h"

#include "LCD.h"

#define FILE_ADDR "/dev/i2c-1"
#define I2C_ADDR 0x27

int LCD::lcdInit(){

    if((file_i2c = open(FILE_ADDR, O_RDWR)) < 0){
        std::cout << "Failed to establish i2c connection!" << std::endl;
        return 1;
    }

    if(ioctl(file_i2c, I2C_SLAVE, I2C_ADDR) < 0){
        std::cout << "Failed to connect i2c devide!" << std::endl;
        close(file_i2c);    
        return 1;
    }


    int buffer[] = {0, 0};
    write(file_i2c, &buffer,1);


    close(file_i2c);

    return 0;
}

int LCD::lcdPrintOut(int packets, int bitRate, units unit){

    return 0;
}

int LCD::clearLCD(){

    return 0;
}

int LCD::sendByte(){


    return 0;
}