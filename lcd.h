
enum units{
    b,
    kb,
    Mb
};

void lcd_init();
int lcdPrintOut(int, int, units);
void clearLCD();