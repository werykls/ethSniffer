#include <iostream>
#include <netinet/in.h>
#include "frame.h"

Frame::Frame(){
    // dhost = 1;
    // shost = 1;
    // type = 1;
}

Frame::Frame(unsigned char buffer[65536]){
    
    for(int i = 0; i < MAC_ADDERSS_SIZE_B; i++){
        dhost[i] = buffer[i];
        shost[i] = buffer[i + 6]; 
    
    }
    type = buffer[12] + buffer[13];
}

uint64_t Frame::getMACnumeric(enum MAC type){
    uint8_t *address;
    if(type == MAC::DESTINATION)address = dhost; 
    else address = shost;
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
        printf("%02X%s", dhost[i], (i < 5)? ":" : " ");
    }
    std::cout << std::endl;
}

void Frame::printSourceMAC(){
    std::cout << "Source MAC: " ;
    for (int i = 0; i < 6; i++){
        printf("%02X%s", shost[i], (i < 5)? ":" : " ");
    }
    std::cout << std::endl;
}

void Frame::printFrameType(){
    std::cout << type << std::endl;
    printf("Protocol/Type : 0x%04X\n", ntohs(type));
}