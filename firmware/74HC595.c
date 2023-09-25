#include "74HC595.h"

void shiftOut(unsigned char dataa) {
    unsigned char i;

    for (i = 0; i < 8; i++) {
        DS = (dataa & 0x80) ? 1 : 0; // Send the most significant bit (MSB) to DS
        SHCP = 1; // Pulse the clock pin to shift in the dataa
        SHCP = 0;
        dataa <<= 1; // Shift dataa left
    }

    STCP = 1; // Pulse the latch pin to update the outputs
    STCP = 0;
}

void displayDigit(unsigned short digit, unsigned char display) {
    unsigned int value;
    unsigned char valueHex;

    value = (int) digit / 10;

    switch (display)
    {
    case 3:
        value = value / 1000;
        if(value == 0) valueHex = convert2Hex(-1);
        else valueHex = convert2Hex(value);
        break;
    case 2:
        value = value / 100;
        value = value % 10;
        valueHex = convert2Hex(value);
        break;
    case 1:
        value = value / 10;
        value = value % 10;
        valueHex = convert2Hex(value);
        break;
    case 0:
        value = value % 10;
        valueHex = convert2Hex(value);
        break;    
    default:
        break;
    }

    // Enable the appropriate display
    W1 = (display == 0) ? 0 : 1;
    W2 = (display == 1) ? 0 : 1;
    W3 = (display == 2) ? 0 : 1;
    W4 = (display == 3) ? 0 : 1;

    //shiftOut(digitPatterns[digit]);
    shiftOut(valueHex);
    delay_us(50);
}

void ledInit() {
    // Configure pins as output
    DS = 0;
    SHCP = 0;
    STCP = 0;

    W1 = 0;
    W2 = 0;
    W3 = 0;
    W4 = 0;
}

unsigned char convert2Hex(int value){
    unsigned char valueHex;

    // 7-segment display digit patterns (common anode)
    //    0xC0, // 0
    //    0xF9, // 1
    //    0xA4, // 2
    //    0xB0, // 3
    //    0x99, // 4
    //    0x92, // 5
    //    0x82, // 6
    //    0xF8, // 7
    //    0x80, // 8
    //    0x90  // 9

    switch (value) {
    case -1:
        valueHex = 0xFF;
        break;
    case 0:
        valueHex = 0xC0;
        break;
    case 1:
        valueHex = 0xF9;
        break;
    case 2:
        valueHex = 0xA4;
        break;
    case 3:
        valueHex = 0xB0;
        break;
    case 4:
        valueHex = 0x99;
        break;
    case 5:
        valueHex = 0x92;
        break;
    case 6:
        valueHex = 0x82;
        break;
    case 7:
        valueHex = 0xF8;
        break;
    case 8:
        valueHex = 0x80;
        break;
    case 9:
        valueHex = 0x90;
        break;
    default:
        valueHex = 0x00;
        break;
    }
    return valueHex;
}



