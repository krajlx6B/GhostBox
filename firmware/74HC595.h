#include "15W408AS.h"
#include "utils.h"
#include "USART.h"
#include "RDA5807M.h"

#define DS P32
#define STCP P33
#define SHCP P34

#define W1 P23
#define W2 P22
#define W3 P21
#define W4 P20

void shiftOut(unsigned char dataa);

void displayFreq();
void displayVol();
void displayDelay(unsigned char value);

void ledInit();

unsigned char convert2Hex(char value);
