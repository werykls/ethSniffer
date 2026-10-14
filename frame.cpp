#include <iostream>
#include <netinet/in.h>
#include "frame.h"


// Primary constructor (passes the buffer)
Frame::Frame(uint8_t *_buffer): buffer(_buffer){}

uint64_t Frame::getMacAddress(enum MAC type){
    uint8_t *address;
    if(type == MAC::DESTINATION)address = buffer; 
    else address = buffer + 6;
    uint64_t output = 0;
     for(int i = 0; i < MAC_ADDERSS_SIZE_B; i++){
        output = output <<  8;
        output += *(address + i);
    }
    return output;
}

// Returns value of a ethernet frame type
uint16_t Frame::getFrameType(){
    uint16_t type;
    type = buffer[12];
    type = type << 8;
    type = type bitor buffer[13];

    return type;
}

// Returns a pointer to a first cell in buffer that contains data encapsulated inside the ethernet frame
uint8_t * Frame:: getDataUnit(){
    return buffer + 14;
}

void Frame::printDestinationMAC(){
    for (int i = 0; i < 6; i++){
        printf("%02X%s", buffer[i], (i < 5)? ":" : " ");
    }
}

void Frame::printSourceMAC(){
    for (int i = 0; i < 6; i++){
        printf("%02X%s", buffer[i + 6], (i < 5)? ":" : " ");
    }
}

void Frame::printFrameType(){
    printf("Type : 0x%04X", Frame::getFrameType());
}