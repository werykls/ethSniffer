#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <thread>
#include <chrono>
#include <string>

#include "lcd.h"


#define I2C_ADDR 0x27 // Change to 0x3f if your i2cdetect address was different
#define LCD_CHR 1     // Mode: data
#define LCD_CMD 0     // Mode: command
#define LCD_BACKLIGHT 0x08
#define ENABLE 0x04

int file_i2c;
int backlight_state = 0x08; // 0x08 is ON, 0x00 is OFF

void delay(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}


void lcd_toggle_enable(int bits) {
    delay(1);
    char data = (bits | ENABLE);
    write(file_i2c, &data, 1);
    delay(1);
    data = (bits & ~ENABLE);
    write(file_i2c, &data, 1);
    delay(1);
}

void lcd_byte(int bits, int mode) {
    int bits_high = mode | (bits & 0xF0) | backlight_state;
    int bits_low = mode | ((bits << 4) & 0xF0) | backlight_state;
    
    char data[1];
    
    data[0] = (char)bits_high;
    write(file_i2c, data, 1);
    lcd_toggle_enable(bits_high);
    
    data[0] = (char)bits_low;
    write(file_i2c, data, 1);
    lcd_toggle_enable(bits_low);
}

void lcd_display_off() {
    backlight_state = 0x00;        // Turn off the LED backlight
    lcd_byte(0x08, LCD_CMD);       // Command to turn off text rendering
}

void lcd_display_on() {
    backlight_state = 0x08;        // Turn on the LED backlight
    lcd_byte(0x0C, LCD_CMD);       // Command to turn on text rendering
}

void lcd_init() {
    lcd_display_on();

    const char* filename = "/dev/i2c-1";
    if ((file_i2c = open(filename, O_RDWR)) < 0) {
        std::cerr << "Failed to open the i2c bus. Try running with sudo or check permissions." << std::endl;
        return;
    }

    if (ioctl(file_i2c, I2C_SLAVE, I2C_ADDR) < 0) {
        std::cerr << "Failed to acquire bus access/talk to slave." << std::endl;
        return;
    }
    


    lcd_byte(0x33, LCD_CMD); // Initialize
    lcd_byte(0x32, LCD_CMD); // Set to 4-bit mode
    lcd_byte(0x06, LCD_CMD); // Cursor move direction
    lcd_byte(0x0C, LCD_CMD); // Turn on display, cursor off
    lcd_byte(0x28, LCD_CMD); // 2 line display, 5x8 font
    lcd_byte(0x01, LCD_CMD); // Clear display
    delay(5);
}

void lcd_string(const char* message, int line) {
    // Line addresses for a 4x20 screen
    if (line == 1) lcd_byte(0x80, LCD_CMD);
    if (line == 2) lcd_byte(0xC0, LCD_CMD);
    if (line == 3) lcd_byte(0x94, LCD_CMD);
    if (line == 4) lcd_byte(0xD4, LCD_CMD);

    while (*message) {
        lcd_byte(*message++, LCD_CHR);
    }
}

int lcdPrintOut(int packets, int bitRate, units unit) {
    

    lcd_string((std::to_string(packets).append(" pack/sec")).c_str(), 1);
    lcd_string((std::to_string(bitRate).append(((unit == units::Mb)? " Mb/s" : (unit ==  units::kb)? " kb/s" : " b/s"))).c_str(), 2);

    return 0;
}

void clearLCD(){
    lcd_display_off();
    close(file_i2c);
    return;
}