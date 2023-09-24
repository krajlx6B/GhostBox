#include "utils.h"

void delay1us(void)	//@18.432MHz
{
	unsigned char data i;

	i = 2;
	while (--i);
}

void delay_us(unsigned int value){
	unsigned int i;
  for (i = 0; i < value; i++) {
      delay1us();
	}
}

void delay_ms(unsigned int value){ 
  while(value > 0){
    --value;
    delay_us(1000);
  }
}
