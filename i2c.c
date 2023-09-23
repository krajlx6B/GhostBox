#include "i2c.h"

bit deviceFound = 0; // Flag to indicate if a device is found

// Function to generate a start condition
void I2C_Start() {
    SDA = 1;
		delay_us(I2C_DELAY_US);
    SCL = 1;
    delay_us(I2C_DELAY_US); // Add a delay for stability
    SDA = 0;
    delay_us(I2C_DELAY_US); // Add a delay for stability
    SCL = 0;
		delay_us(I2C_DELAY_US);
}

// Function to generate a stop condition
void I2C_Stop() {
    SDA = 0;
		delay_us(I2C_DELAY_US);
    SCL = 1;
    delay_us(I2C_DELAY_US); // Add a delay for stability
    SDA = 1;
    delay_us(I2C_DELAY_US); // Add a delay for stability
}

void I2C_send_ack() {
    SDA = 0;  // Pull SDA low to send ACK
		delay_us(I2C_DELAY_US);
    SCL = 1;  // Generate a clock pulse
		delay_us(I2C_DELAY_US);
    // Add a small delay here if needed for timing requirements
    SCL = 0;  // Bring the clock line back low
		delay_us(I2C_DELAY_US);
    SDA = 1;  // Release SDA
		delay_us(I2C_DELAY_US);
}

void I2C_send_nack() {
    SDA = 1;  // Release SDA (high) to send NACK
		delay_us(I2C_DELAY_US);
    SCL = 1;  // Generate a clock pulse
		delay_us(I2C_DELAY_US);
    // Add a small delay here if needed for timing requirements
    SCL = 0;  // Bring the clock line back low
		delay_us(I2C_DELAY_US);
}

// Function to receive a byte
unsigned char I2C_ReadByte() {
    unsigned char i, dat = 0;
	
    SDA = 1; // Release data line
	
    for (i = 0; i < 8; i++) {
        dat <<= 1;     // Shift left
        SCL = 1;        // Clock high
        delay_us(I2C_DELAY_US);        // Add a delay for stability
        if (SDA) dat |= 0x01; // Read the bit
        SCL = 0;        // Clock low
				delay_us(I2C_DELAY_US);  
    }
    return dat;
}

unsigned char I2C_WriteByte(unsigned char dat) {
    unsigned char i;
    for (i = 0; i < 8; i++) {
        SDA = dat & 0x80;
        dat <<= 1;
        SCL = 1;
        delay_us(I2C_DELAY_US);  
        SCL = 0;
				delay_us(I2C_DELAY_US);  
    }
    SDA = 1;
    delay_us(I2C_DELAY_US);  
    SCL = 1;
    delay_us(I2C_DELAY_US);  
    if (SDA) {
        SCL = 0;
				delay_us(I2C_DELAY_US);  
        return 1; // NACK received
    } else {
        SCL = 0;
				delay_us(I2C_DELAY_US);  
        return 0; // ACK received
    }
}

unsigned char I2C_Scan() {
  unsigned char address;
  unsigned char ack;

	for(address = 1; address < 128; address++){
		I2C_Start();
		ack = I2C_WriteByte(address << 1);
		I2C_Stop();
	
		if (ack == 0) return address;
	}
  return 0; // Return TRUE if a device is found, otherwise FALSE
}

//Delays
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
