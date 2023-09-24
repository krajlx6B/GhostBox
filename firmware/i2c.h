#include "15W408AS.h"
#include "utils.h"

#define SDA P24
#define SCL P25

#define I2C_DELAY_US 5

void I2C_Start();
void I2C_Stop();

void I2C_send_ack();
void I2C_send_nack();

unsigned char I2C_ReadByte();
unsigned char I2C_WriteByte(unsigned char dat);
unsigned char I2C_Scan();

