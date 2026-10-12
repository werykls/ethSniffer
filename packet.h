#include <cstdint>
#include <string>

#define IPv4_ADDRESS_SIZE_B 4
#define IPv6_ADDRESS_SIZE_B 16

enum class IP{
    SOURCE,
    DESTINATION
};

class Packet{
    uint8_t *_buffer;

    public:

        static const uint8_t headerSize = 20;

        uint8_t ip_dhost[IPv4_ADDRESS_SIZE_B];
        uint8_t ip_shost[IPv4_ADDRESS_SIZE_B];
        uint8_t *dataUnit;

        Packet(uint8_t *);

        uint8_t getVersion();
        uint8_t getIHL();
        uint8_t getDSCP();
        uint8_t getECN();
        uint16_t getTotalLength();
        uint16_t getIdentification();
        uint8_t getFlags();
        uint16_t getFragmentOffset();
        uint8_t getTTL();
        uint8_t getProtocol();
        uint16_t getHeaderChecksum();

        uint32_t getIPAddress(IP);
        void printIPAddress(IP);

        static std::string getProtocolName(uint8_t);

};