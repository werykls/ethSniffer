// Standart libraries
#include <iostream>
#include <unistd.h>
#include <iomanip>
#include <string.h>

//f etching network interfaces names
#include <ifaddrs.h>



// Socket handling
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <linux/if_packet.h>
#include <net/if.h>

// Time and threads
#include <chrono>
#include <atomic>
#include <thread>




#include "frame.h"
#include "packet.h"
#include "TCP.h"

// #define debug
// #define fileDescriptionManipulations

void getInterfaces();
std::string interface;

int raw_socket; //socket
int packetLimiter {0}; // The limit of packets to capture provided by user
std::atomic<bool> programStatus {true};

enum filtering{
    ALL,
    OUTGOING,
    LOOPBACK,
    SNIFFED
};

// Buffer to hold the incoming packet data
unsigned char buffer[65536];
ssize_t dataSize;
std::atomic<int> packetNumber;
std::atomic<long> bitRate;


enum filtering filter;


int init(int, char *[]);
void printDataUnit(uint8_t *,int , int);
void printLog();
void logPrinter(std::atomic<bool> &);
// Prints a full informations details about captured traffic
void printFrame(int, Frame &, uint8_t *, ssize_t);

int trafficHandler();

int main(int args, char *argv[]){
    
    if(int er = init(args, argv))return er;
    

    std::cout << "Listening for incoming Ethernet frames..." << std::endl;
    
    std::thread timer_thread(logPrinter, std::ref(programStatus));

    while(programStatus){
        
        trafficHandler();
        
    }


    timer_thread.join();
    close(raw_socket);
    return 0;
}


void getInterfaces() 
{
    struct ifaddrs * addrs;
    getifaddrs(&addrs);


    std::cout << "Found interfaces: " << std::endl;
    for (struct ifaddrs *addr = addrs; addr != nullptr; addr = addr->ifa_next) {
        if (addr->ifa_addr && addr->ifa_addr->sa_family == AF_PACKET) {
            std::cout << addr->ifa_name << std::endl;
        }
    }

    freeifaddrs(addrs);

    return;
}

int init(int args, char *argv[]){
    
    

    getInterfaces();
    std::cin >> interface;
    
    

    // Create a raw socket to capture all Ethernet frames (ETH_P_ALL)
    // htons() converts the constant to network byte order
    raw_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    struct packet_mreq mr;
    memset(&mr, 0, sizeof(mr));
    mr.mr_ifindex = if_nametoindex("eth0");

    mr.mr_type = PACKET_MR_PROMISC;
    if(setsockopt(raw_socket, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) < 0){
        std::cout << "Socket failed to enable promiscuous mode!!!" << std::endl;
        return 1;
    }

    if (raw_socket < 0) {
        std::cerr << "Error: Could not create socket. Are you running as root/sudo?" << std::endl;
        return 1;
    }

    packetLimiter = std::stoi(argv[2]);

    std::cout << "Filtering mode: ";
    if(std::string_view(argv[1]) == "ALL"){
        std::cout << "ALL" << std::endl;
        filter = filtering::ALL;
        return 0;
    }
    else if(std::string_view(argv[1]) == "OUTGOING"){
        std::cout << "OUTGOING" << std::endl;
        filter = filtering::OUTGOING;
        return 0;
    }
    else if(std::string_view(argv[1]) == "LOOPBACK"){
        std::cout << "LOOPBACK" << std::endl;
        filter = filtering::LOOPBACK;
        return 0;
    }
    else if(std::string_view(argv[1]) == "SNIFFED"){
        std::cout << "SNIFFED" << std::endl;
        filter = filtering::SNIFFED;
        return 0;
    }
    

    #ifdef fileDescriptionManipulations
    // changing socket mode to BLOCKING
    int flags = fcntl(raw_socket, F_GETFL, 0);
    std::cout << flags << std::endl;
    (flags & O_NONBLOCK)?std::cout << "Socket nie blokujacy": std::cout << "Socket blokujacty"; std::cout << std::endl;
    //(fcntl(raw_socket, F_SETFL, flags bitor O_NONBLOCK) == 0)? std::cout << "udalo sie zmienic" : std::cout << "nie udalo sie zmienic"; std::cout<< std::endl;
    
    flags = fcntl(raw_socket, F_GETFL, 0);
    std::cout << "po zmianie || " << (flags bitor O_NONBLOCK) << std::endl;
    std::cout << flags << std::endl;
    (flags & O_NONBLOCK)?std::cout << "Socket nie blokujacy": std::cout << "Socket blokujacty"; std::cout << std::endl;
    #endif

    return 0;
}

void logPrinter(std::atomic<bool>& status){

    auto nextTick = std::chrono::steady_clock::now();
    
    while(status){
        nextTick += std::chrono::seconds(1);
        std::cout << std::dec << packetNumber << "packets/sec; " << bitRate << " bits/sec" << std::endl;
        packetNumber = 0;
        bitRate = 0;

        std::this_thread::sleep_until(nextTick);
    }
}

int trafficHandler(){
    // Receive data from the socket
    dataSize = recvfrom(raw_socket, buffer, sizeof(buffer), 0, nullptr, nullptr);

    // The limiter for packet retriving passed by argv
    if(packetNumber == packetLimiter){
        programStatus = false;
        return 0;
    }
    
    
    if(dataSize < 0){
        std::cout << "Failed to receive data" << dataSize << std::endl;
        return 1;
    }
    
    
    //Custom object for handling frame's data
    Frame frame = Frame(buffer);
    
    // eth frames basic filtering 
    switch(filter)
    {
        case filtering::OUTGOING: {
            if(frame.getMacAddress(MAC::DESTINATION) == 0 and frame.getMacAddress(MAC::SOURCE) == 0)return 0;
        }
        case filtering::LOOPBACK: {
            if(frame.getMacAddress(MAC::DESTINATION) != 0 and frame.getMacAddress(MAC::SOURCE) != 0)return 0;
        }
        case filtering::SNIFFED: {
            if(frame.getMacAddress(MAC::DESTINATION) == 0 and frame.getMacAddress(MAC::SOURCE) == 0)return 0;
            if(frame.getMacAddress(MAC::DESTINATION) == 0x2ccf67309277 or frame.getMacAddress(MAC::SOURCE) == 0x2ccf67309277)return 0; 
        }
        case filtering::ALL: {
            break;
        }
    }
    
    packetNumber++;
    bitRate += dataSize;

    std::cout << std::hex << frame.getMacAddress(MAC::DESTINATION) << " | ";
    std::cout << std::hex << frame.getMacAddress(MAC::SOURCE) << std::endl;

    // printFrame(packetNumber, frame, buffer, dataSize);
    // printDataUnit(buffer, dataSize, 0);
    
    // We are updating packets counter after filtering out loopback traffic

    return 0;
}


void printFrame(int packetNumber, Frame & frame, uint8_t * buffer, ssize_t dataSize){
    
    std::cout << "\n" << std::setw(250) << std::setfill('-') << std::dec << "\n";
    std::cout << std::setfill(' ')<< std::setw(3) << packetNumber << " | ";
    std::cout << "Size: " << dataSize << " bytes" << " | ";
    
    
    // printing ethernet frames header 
    std::cout << "Des MAC: " ;
    frame.printDestinationMAC();
    std::cout << " | Src MAC: " ;
    frame.printSourceMAC();
    std::cout << " | ";
    frame.printFrameType();
    std::cout << " | ";

    

    Packet packet = Packet(frame.getDataUnit());
    // printing IP packet haeder
    std::cout << "Src IP: ";
    packet.printIPAddress(IP::SOURCE);
    std::cout << "| Des IP: ";
    packet.printIPAddress(IP::DESTINATION);
    std::cout << std::hex << " | Encapsulated protocol ID: " << Packet::getProtocolName(packet.getProtocol());
    std::cout << " | ";

    
    TCP tcp = TCP(&buffer[Frame::headerSize + packet.headerSize]);
    std::cout << std::dec;
    std::cout << "Src socket nr: " << tcp.getSocketNumber(SOCKET::SOURCE);
    std::cout << " | ";
    std::cout << "Des socket nr: " << tcp.getSocketNumber(SOCKET::DESTINATION);
    
    
    // printing full data buffer
    std::cout << std::setw(250) << std::setfill('-') << std::dec << "\n";
    

    std::cout << "\n" << std::setw(250) << std::setfill('-') << std::dec << "\n";
    return;
}

void printDataUnit(uint8_t buffer[],int dataSize, int startingIndex = 0){
    for(int i = startingIndex; i < dataSize - 1; i++){
        std:: cout << std::setw(2) << std::setfill('0') << std::hex <<  int(buffer[i]) << " ";
    }
}