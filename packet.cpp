#include "packet.h"
#include <iostream>

Packet::Packet(uint8_t *_buffer): buffer(_buffer){
    if(getIHL() > 5)headerSize = 0;
    else return;
}

uint8_t Packet::getVersion(){
    return  buffer[0] >> 4;
}

uint8_t Packet::getIHL(){
    return buffer[0] bitand 0xf;
}

uint8_t Packet::getDSCP(){
    return buffer[1] >> 2;
}


uint8_t Packet::getECN(){
    return buffer[1] bitand 0b11;
}

uint16_t Packet::getTotalLength(){
    uint16_t ttl;
    ttl = buffer[2];
    ttl = ttl << 8;
    ttl = ttl bitor buffer[3];
    return ttl;
}

uint16_t Packet::getIdentification(){
    uint16_t identification;
    identification = buffer[4];
    identification = identification << 8;
    identification = identification bitor buffer[5];
    return identification;
}

uint8_t Packet::getFlags(){
    return buffer[6] >> 5;
}

uint16_t Packet::getFragmentOffset(){
    uint16_t fragmentOffset;
    fragmentOffset = buffer[6] bitand 0b11111;
    fragmentOffset = fragmentOffset << 8;
    fragmentOffset = fragmentOffset bitor buffer[7];
    return fragmentOffset;
}

uint8_t Packet::getTTL(){
    return buffer[8];
}

uint8_t Packet::getProtocol(){
    return buffer[9];
}

uint16_t Packet::getHeaderChecksum(){
    uint16_t HeaderChecksum;
    HeaderChecksum = buffer[10];
    HeaderChecksum = HeaderChecksum << 8;
    HeaderChecksum = HeaderChecksum bitor buffer[11];
    return HeaderChecksum;
}

uint32_t Packet::getIPAddress(IP ipAddressType){
    uint32_t ipAddress;
    uint8_t slider;
    if(ipAddressType == IP::SOURCE)slider = 0;
    else slider = 4;
    for(uint8_t i = 0; i < 4; i++){
        ipAddress = ipAddress << 8;
        ipAddress = ipAddress bitor buffer[12 + i + slider];
    }
    return ipAddress;
}

void Packet::printIPAddress(enum IP ipAddessType){
    uint32_t ipAddress = getIPAddress(ipAddessType);
    uint8_t octet;
    std::cout << std::dec;
    for(int i = 3; i > -1; i--){
        octet = ipAddress >> 8 * i;
        printf("%u%s", octet, (i != 0)? ":": " ");
    }

}

std::string Packet::getProtocolName(uint8_t protocolNumber){
    switch (protocolNumber)
    {
        case 1:
            return "ICMP";
            break;
        case 2:
            return "IGMP";
            break;
        case 6:
            return "TCP";
            break;
        case 17:
            return "UDP";
            break;
        case 41:
            return "ENCAP \"IPv6 encapsulation\"";
            break;
        
        default:
            return "Unknown";
            break;
        }
}