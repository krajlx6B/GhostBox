#include <intrins.h>
#include <stdio.h>
#include <stdarg.h>
#include "15W408AS.h"

#define FOSC 18432000L //System frequency  18.432MHz
#define BAUD 115200 //UART1 baud-rate

void UartInit(void);
void SendChar(unsigned char dat);
void SendString(char *s);
void SendFloat(float num);
void PrintSerial(const char *format, ...);
