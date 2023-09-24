/*

SBUF: UART1 uses register SBUF (address:99H) to hold both the received and transmitted data passing through pins RxD and TxD.all have two in the chip, one for transmit and the other for receive(OK)

PCON&SCON:Serial port 1 of STC15 series has two control registers: Serial port control register (SCON) and PCONwhich used to select Baud-Rate
SCON:SM0  SM1  Mode  BaudRate
            0    1     1  (T1 or T2)/4 (OK)
            0    0     0   SYSclk/12
PCON:
B7-SMOD
 0 : Disable double Baud rate of the UART(OK)
 1 : Enable double Baud rate of the UART in mode 1,2,or 3.
B6-SMOD0
 0 : SCON.7 is SM0 function(OK)
 1 : SCON.7 is FE function. Note that FE will be set after a frame error regardless of the state of SMOD0.

AUXR:
B6-T1x12:Timer 1 clock source bit
 0 : The clock source of Timer 1 is SYSclk/12. It will compatible to the traditional 8051 MCU
 1 : The clock source of Timer 1 is SYSclk/1. It will drive the T0 faster than a traditional 8051 MCUIf T1 is used as the baud-rate generator of UART1, T1x12 will decide whether UART1 is 1T or 12T.(OK)
B0 - S1ST2 : the control bit that UART1 select Timer 2 as its baud-rate generator.
 0 : Select Timer 1 as the baud-rate generator of UART1(OK)
 1 : Select Timer 2 as the baud-rate generator of UART1. Timer 1 is released to use in other functions

IE:
B7-EA : disables all interrupts.
 0 : no interrupt will be acknowledged.
 1 : each interrupt source is individually enabled or disabled by setting or clearing its enable bit.(OK)
B4-ES : Serial port 1(UART1) interrupt enable bit.
 0 : Serial port 1(UART1) interrupt would be diabled.
 1 : Serial port 1(UART1) interrupt would be enabled.(OK)

Timer1 Configuration：
 #define    Timer1_Load(n)    TH1 = (n)/256,TL1 = (n)%256 Use this sentence to set the T1.So we need to calculate the buad_rate we need！


综合上述的寄存器的配置，我们将选择Timer1来产生USART1的波特率的时钟源，我们选择的Serial Port的模式Mode=1,当数据传输完成时会产生中断，
我们可以通过检查中断向量TI(SCON-B1)的值来判断数据的传输是否完成，完成则为1，未完成则为0：while(TI == 0); --> TI = 0;(不要忘记清零)

那么波特率的计算如下：Buad_Rate = T1/4
设定的Timer1时钟如下：Time_Clk = SYSclk/1
*/
#include "USART.h"

void UartInit(void) {
    SCON  = (SCON & 0x3f) | 0x40; //选择usart1的mode为1
    S1_USE_P30P31();
    Timer2_1T(); //The clock source of Timer 2 is SYSclk/1.
    Timer2_AsTimer(); // TMOD &= ~(1<<6) 定时器1用做定时器
    S1_BRT_UseTimer2(); //Select Timer 1 as the baud-rate generator
    T2L = (65536 - (FOSC/4/BAUD)); //Set the preload value
    T2H = (65536 - (FOSC/4/BAUD))>>8;
    EA = 1;
    ES = 1;
    Timer2_Run(); // 启动定时器1
}

void SendChar(unsigned char dat) {
    SBUF = dat;
    while(!TI);
    TI = 0;
}

void SendString(char *s) {
    while (*s)  {
        SendChar(*s++);
    }
}

void SendFloat(float num) {
    int a,b;
    a = (int)num;
    b = (int)((num-a)*10);
    SendChar((unsigned char)(a+48));
    SendChar('.');
    SendChar((unsigned char)(b+48));
    SendChar('\n');
}

void PrintSerial(const char *format, ...) {
    int i;
    char buffer[30]; 

    va_list args;
    va_start(args, format);

    for (i = 0; i < 30; i++) buffer[i] = '\0'; 

    vsprintf(buffer, format, args);
    va_end(args);
    SendString(buffer);
}