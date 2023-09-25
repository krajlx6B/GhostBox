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

void displayFreq() {
    unsigned char i;
    unsigned int freq;
    char value;

    freq = (int) getFreq() / 10;

    for(i=0;i<4;i++){
        switch (i)
        {
        case 3:
            value = freq / 1000;
            if(value == 0) value = -1;
            break;
        case 2:
            value = freq / 100;
            value = value % 10;
            break;
        case 1:
            value = freq / 10;
            value = value % 10;
            break;
        case 0:
            value = freq % 10;
            break;    
        default:
            break;
        }

        // Enable the appropriate display
        W1 = (i == 0) ? 0 : 1;
        W2 = (i == 1) ? 0 : 1;
        W3 = (i == 2) ? 0 : 1;
        W4 = (i == 3) ? 0 : 1;

        //shiftOut(valueHex);
        shiftOut(convert2Hex(value));
        delay_ms(1);
    }
}

void displayVol() {
    unsigned char i;
    unsigned int vol;
    char value;

    vol = (unsigned int) getVolume();

    for(i=0;i<4;i++) {
        switch (i)
        {
        case 3:
            value = -2;
            break;
        case 2:
            value = -1;
            break;
        case 1:
            value = vol / 10;
            //value = value % 10;
            break;
        case 0:
            value = vol % 10;
            break;    
        default:
            break;
        }

        // Enable the appropriate display
        W1 = (i == 0) ? 0 : 1;
        W2 = (i == 1) ? 0 : 1;
        W3 = (i == 2) ? 0 : 1;
        W4 = (i == 3) ? 0 : 1;

        shiftOut(convert2Hex(value));
        delay_ms(1);
    }
}

void displayDelay(unsigned char delay){
    unsigned char i;
    char value;

    value = delay;

    for(i=0;i<4;i++) {
        switch (i)
        {
        case 3:
            value = -3;
            break;
        case 2:
            value = delay / 100;
            break;
        case 1:
            value = delay / 10;
            value = value % 10;
            break;
        case 0:
            value = delay % 10;
            break;    
        default:
            break;
        }

        // Enable the appropriate display
        W1 = (i == 0) ? 0 : 1;
        W2 = (i == 1) ? 0 : 1;
        W3 = (i == 2) ? 0 : 1;
        W4 = (i == 3) ? 0 : 1;

        shiftOut(convert2Hex(value));
        delay_ms(1);
    }
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

unsigned char convert2Hex(char value){
    unsigned char valueHex;

    switch (value) {
    case -3:
        valueHex = 0xA1;    //d
        break;
    case -2:
        valueHex = 0xC1;    //v
        break;
    case -1:
        valueHex = 0xFF;    //null
        break;
    case 0:
        valueHex = 0xC0;    //0
        break;
    case 1:
        valueHex = 0xF9;    //1
        break;
    case 2:
        valueHex = 0xA4;    //2
        break;
    case 3:
        valueHex = 0xB0;    //3
        break;
    case 4:
        valueHex = 0x99;    //4
        break;
    case 5:
        valueHex = 0x92;    //5
        break;
    case 6:
        valueHex = 0x82;    //6
        break;
    case 7:
        valueHex = 0xF8;    //7
        break;
    case 8:
        valueHex = 0x80;    //8
        break;
    case 9:
        valueHex = 0x90;    //9
        break;
    default:
        valueHex = 0x00;    //all
        break;
    }
    return valueHex;
}



