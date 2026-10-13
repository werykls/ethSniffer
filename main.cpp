#include <iostream>
#include <unistd.h>
#include <iomanip>

#include <ifaddrs.h> //fetching network interfaces names
// #include <fcntl.h> //socket files descriptors manipulation


#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>

#include "frame.h"
#include "packet.h"
#include "TCP.h"

// #define debug
// #define fileDescriptionManipulations

int getInterfaces();
std::string interface;

int raw_socket; //socket
int packetLimiter; // The limit of packets to capture provided by user

enum filtering{
    ALL,
    OUTGOING,
    LOOPBACK
};

enum filtering filter;

void printDataUnit(uint8_t buffer[],int data_size, int startingIndex = 0){
    for(int i = startingIndex; i < data_size - 1; i++){
        std:: cout << std::setw(2) << std::setfill('0') << std::hex <<  int(buffer[i]) << " ";
    }
    std::cout << std::endl;
}

int init(int args, char *argv[]){
    
    // getInterfaces();
    // std::cin >> interface;
    
    // Create a raw socket to capture all Ethernet frames (ETH_P_ALL)
    // htons() converts the constant to network byte order
    raw_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
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



int main(int args, char *argv[]){

    if(int er = init(args, argv))return er;
    std::cout << "Listening for incoming Ethernet frames..." << std::endl;
    
    // Buffer to hold the incoming packet data
    unsigned char buffer[65536];
    
    int packetNumber = 0;
    ssize_t data_size;
    while(true){
        // The limiter for packet retriving passed by argv
        if(packetNumber == packetLimiter)return 0;


        // Receive data from the socket
        data_size = recvfrom(raw_socket, buffer, sizeof(buffer), 0, nullptr, nullptr);
        
        
        if(data_size < 0){
            std::cout << "Failed to receive data" << data_size << std::endl;
            continue;
        }
        
        // Previously used
        // Cast the beginning of the buffer to an Ethernet header struct
        // struct ether_header *eth_hdr = reinterpret_cast<struct ether_header *>(buffer);
        
        //Custom object for handling frame's data
        Frame frame = Frame(buffer);
        
        //eth frames basic filtering 
        if(filter == filtering::OUTGOING){
            if(frame.getMacAddress(MAC::DESTINATION) == 0 and frame.getMacAddress(MAC::SOURCE) == 0)continue;
        }
        std::cout << std::dec;
        std::cout << "\nPakiet nr: " << packetNumber << std::endl;
        packetNumber++;
        
        
        
        std::cout << "\nPacket captured! Size: " << data_size << " bytes" << std::endl;
        
        // printing full data buffer
        // printDataUnit(buffer, data_size, Frame::headerSize + packet.headerSize);
        
        Packet packet = Packet(frame.getDataUnit());
        
        
        
        // printing ethernet frames header 
        frame.printDestinationMAC();
        frame.printSourceMAC();
        frame.printFrameType();
        
        // printing IP packet haeder
        packet.printIPAddress(IP::SOURCE);
        packet.printIPAddress(IP::DESTINATION);
        
        

        std::cout << std::hex << "IP packet protocol ID: " << Packet::getProtocolName(packet.getProtocol()) << std::endl;

        TCP tcp = TCP(&buffer[Frame::headerSize + packet.headerSize]);
        std::cout << std::dec;
        std::cout << "Source socket nr: " << tcp.getSocketNumber(SOCKET::SOURCE) << std::endl;
        std::cout << "Destination socket nr: " << tcp.getSocketNumber(SOCKET::DESTINATION) << std::endl;
        
    }


    close(raw_socket);
    return 0;
}


int getInterfaces()
{
    struct ifaddrs *addrs;
    getifaddrs(&addrs);


    std::cout << "Found interfaces: " << std::endl;
    for (struct ifaddrs *addr = addrs; addr != nullptr; addr = addr->ifa_next) {
        if (addr->ifa_addr && addr->ifa_addr->sa_family == AF_PACKET) {
            std::cout << addr->ifa_name << std::endl;
        }
    }

    freeifaddrs(addrs);

    return 0;
}