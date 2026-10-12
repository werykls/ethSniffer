#include "TCP.h"
#include <iostream>


uint16_t TCP::getSocketNumber(enum SOCKET socketItem){
    uint8_t slider = 0;
    if(socketItem == SOCKET::DESTINATION) slider = 2;
    uint16_t socketNumber;
    socketNumber = buffer[0 + slider];
    socketNumber = socketNumber << 8;
    socketNumber = socketNumber bitor buffer[1 + slider];
    return socketNumber;
}

uint16_t getDestinationSocketNumber();
uint32_t getSequenceNumber();