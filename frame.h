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

        Frame(uint8_t *);
        
        uint64_t getMacAddress(enum MAC);
        uint16_t getFrameType();
        uint8_t * getDataUnit();
        
        void printDestinationMAC();
        void printSourceMAC();
        void printFrameType();
        

};

