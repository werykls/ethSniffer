#include "../structures.h"

class LCD{
    public:
        int lcdInit();
        int lcdPrintOut(int packets, int bitRate, units unit);
        int clearLCD();
    
    private:
        int sendByte();
        int file_i2c;
        
};
