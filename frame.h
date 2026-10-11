#include <cstdint>

#define MAC_ADDERSS_SIZE_B 6

enum MAC{
    DESTINATION,
    SOURCE
};

class Frame{
    public: 
        uint8_t dhost[MAC_ADDERSS_SIZE_B];
        uint8_t shost[MAC_ADDERSS_SIZE_B];
        uint8_t type;
    
        Frame();
        Frame(unsigned char []);

        void printDestinationMAC();
        void printSourceMAC();
        void printFrameType();
        
        uint64_t getMACnumeric(MAC); 

};