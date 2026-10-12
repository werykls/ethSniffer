#include <iostream>
#include <netinet/in.h>
#include "frame.h"

Frame::Frame(){
    // mac_dhost = 1;
    // mac_shost = 1;
    // type = 1;
}

Frame::Frame(unsigned char buffer[65536]){
    
    for(int i = 0; i < MAC_ADDERSS_SIZE_B; i++){
        mac_dhost[i] = buffer[i];
        mac_shost[i] = buffer[i + 6]; 
    
    }
    // Casting into Little-endian notation
    type = buffer[12];
    type = type << 8;
    type = type bitor buffer[13];

    dataUnit = &buffer[14];
}

uint64_t Frame::getMACnumeric(enum MAC type){
    uint8_t *address;
    if(type == MAC::DESTINATION)address = mac_dhost; 
    else address = mac_shost;
    uint64_t output = 0;
     for(int i = 0; i < MAC_ADDERSS_SIZE_B; i++){
        output = output <<  8;
        output += *(address + i);
    }
    return output;
}

void Frame::printDestinationMAC(){
    std::cout << "Destination MAC: " ;
    for (int i = 0; i < 6; i++){
        printf("%02X%s", mac_dhost[i], (i < 5)? ":" : " ");
    }
    std::cout << std::endl;
}

void Frame::printSourceMAC(){
    std::cout << "Source MAC: " ;
    for (int i = 0; i < 6; i++){
        printf("%02X%s", mac_shost[i], (i < 5)? ":" : " ");
    }
    std::cout << std::endl;
}

void Frame::printFrameType(){
    printf("Protocol/Type : 0x%04X\n", type);
}