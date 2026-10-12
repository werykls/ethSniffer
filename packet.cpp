#include "packet.h"
#include <iostream>

Packet::Packet(uint8_t *buffer): _buffer(buffer){
    dataUnit = &buffer[20];

}

uint8_t Packet::getVersion(){
    return  _buffer[0]>> 8;
}

uint8_t Packet::getIHL(){
    return _buffer[0] bitand 0xf;
}

uint8_t Packet::getDSCP(){
    return _buffer[1] >> 2;
}


uint8_t Packet::getECN(){
    return _buffer[1] bitand 0b11;
}

uint16_t Packet::getTotalLength(){
    uint16_t ttl;
    ttl = _buffer[2];
    ttl = ttl << 8;
    ttl = ttl bitor _buffer[3];
    return ttl;
}

uint16_t Packet::getIdentification(){
    uint16_t identification;
    identification = _buffer[4];
    identification = identification << 8;
    identification = identification bitor _buffer[5];
    return identification;
}

uint8_t Packet::getFlags(){
    return _buffer[6] >> 5;
}

uint16_t Packet::getFragmentOffset(){
    uint16_t fragmentOffset;
    fragmentOffset = _buffer[6] bitand 0b11111;
    fragmentOffset = fragmentOffset << 8;
    fragmentOffset = fragmentOffset bitor _buffer[7];
    return fragmentOffset;
}

uint8_t Packet::getTTL(){
    return _buffer[8];
}

uint8_t Packet::getProtocol(){
    return _buffer[9];
}

uint16_t Packet::getHeaderChecksum(){
    uint16_t HeaderChecksum;
    HeaderChecksum = _buffer[10];
    HeaderChecksum = HeaderChecksum << 8;
    HeaderChecksum = HeaderChecksum bitor _buffer[11];
    return HeaderChecksum;
}

uint32_t Packet::getIPAddress(IP ipAddressType){
    uint32_t ipAddress;
    uint8_t slider;
    if(ipAddressType == IP::SOURCE)slider = 0;
    else slider = 4;
    for(uint8_t i = 0; i < 4; i++){
        ipAddress = ipAddress << 8;
        ipAddress = ipAddress bitor _buffer[12 + i + slider];
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
    std::cout << std::endl;

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