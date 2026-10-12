#include <cstdint>

#define MAC_ADDERSS_SIZE_B 6

enum class MAC{
    DESTINATION,
    SOURCE
};

class Frame{
    uint8_t *buffer;
    public: 
        static const uint8_t headerSize = 14;
        uint8_t mac_dhost[MAC_ADDERSS_SIZE_B];
        uint8_t mac_shost[MAC_ADDERSS_SIZE_B];
        uint16_t type;
        uint8_t *dataUnit;

        Frame();
        Frame(unsigned char []);

        void printDestinationMAC();
        void printSourceMAC();
        void printFrameType();
        
        uint64_t getMACnumeric(MAC); 

};

