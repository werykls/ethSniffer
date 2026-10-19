#include <cstdint>

enum class SOCKET{
    SOURCE,
    DESTINATION
};

class TCP{
    uint8_t *buffer;
    
    public:
        TCP(uint8_t *_buffer): buffer(_buffer){};

        uint16_t getSocketNumber(enum SOCKET);
        uint32_t getSequenceNumber();


};

