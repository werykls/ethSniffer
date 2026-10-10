#include <iostream>
#include <unistd.h>

#include <ifaddrs.h> //fetching network interfaces names

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>


int getInterfaces();
std::string interface;

int raw_socket;

int init(){
    
    getInterfaces();
    // std::cin >> interface;
    
    // Create a raw socket to capture all Ethernet frames (ETH_P_ALL)
    // htons() converts the constant to network byte order
    raw_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));




    if (raw_socket < 0) {
        std::cerr << "Error: Could not create socket. Are you running as root/sudo?" << std::endl;
        return 1;
    }

    return 0;
}



int main(int args, char *chars[]){

    if(int er = init())return er;

    std::cout << "Listening for incoming Ethernet frames..." << std::endl;
    
    
    // Buffer to hold the incoming packet data
    unsigned char buffer[65536];
    
    while(true){
        //Receive data from the socket
        ssize_t data_size = recvfrom(raw_socket, buffer, sizeof(buffer), 0, nullptr, nullptr);
        

        if(data_size < 0){
            std::cout << "Failed to receive data" << std::endl;
            break;
        }

        //Cast the beginning of the buffer to an Ethernet header struct
        struct ether_header *eth_hdr = reinterpret_cast<struct ether_header *>(buffer);

        std::cout << "Packet captured! Size: " << data_size << "bytes" << std::endl;

        //Print Destination MAC address
        std::cout << "Destination MAC: " ;
        for (int i = 0; i < 6; i++){
            printf("%02X%s", eth_hdr -> ether_dhost[i], (i < 5)? ":" : " ");
        }
        std::cout << std::endl;

        //Print Source MAC address
        std::cout << "Source MAC: " ;
        for (int i = 0; i < 6; i++){
            printf("%02X%s", eth_hdr -> ether_shost[i], (i < 5)? ":" : " ");
        }
        std::cout << std::endl;

        //Print the Ethernet type
        printf("Protocol/Type : 0x%04X\n", ntohs(eth_hdr -> ether_type));

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