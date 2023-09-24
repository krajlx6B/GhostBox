#include <stdio.h>
#include "i2c.h"
#include "USART.h"
#include "utils.h"

#define Reg02Address	0x02
#define RADIO_REG_CTRL_OUTPUT 0x8000
#define RADIO_REG_CTRL_UNMUTE 0x4000
#define RADIO_REG_CTRL_MONO 0x2000
#define RADIO_REG_CTRL_BASS 0x1000
#define RADIO_REG_CTRL_SEEKUP 0x0200
#define RADIO_REG_CTRL_SEEK 0x0100
#define RDA_REG_SEEK_STOP   0xFCFF
#define RADIO_REG_CTRL_RDS 0x0008
#define RADIO_REG_CTRL_NEW 0x0004
#define RADIO_REG_CTRL_RESET 0x0002
#define RADIO_REG_CTRL_ENABLE 0x0001

#define Reg03Address	0x03
#define RADIO_REG_CHAN_SPACE 0x0003
#define RADIO_REG_CHAN_SPACE_100 0x0000
#define RADIO_REG_CHAN_BAND 0x000C
#define RADIO_REG_CHAN_BAND_FM 0x0000
#define RADIO_REG_CHAN_BAND_FMWORLD 0x0008
#define RADIO_REG_CHAN_TUNE 0x0010
#define RADIO_REG_CHAN_NR 0x7FC0

#define Reg04Address	0x04
#define RADIO_REG_R4_EM50 0x0800
#define RADIO_REG_R4_SOFTMUTE 0x0200
#define RADIO_REG_R4_AFC 0x0100

#define Reg05Address	0x05
#define RADIO_REG_VOL_VOL 0x000F

#define RADIO_REG_R4_EM50 0x0800 //Set de emphasis at 50 us

#define RADIO_DEFAULT_VOL	0x0000
#define RADIO_DEFAULT_FRQ   9300 //Sun fm

//Sequential read/write mode
#define SEQ_WriteAdr  0x20
#define SEQ_ReadAdr   0x21

//Random Access read/write mode
#define RND_WriteAdr  0x22
#define RND_ReadAdr   0x23

void radioInit(void);

unsigned short readRegisterRan(unsigned char regAddress);
void writeRegisterRan(unsigned char regAddress, unsigned short dat);

void readRegisters(void);
void writeRegisters(void);

void reset(void);

void setFreq(unsigned int freq);
unsigned short getFreq(void);

void seekUp(void);
void seekDown(void);

void setVolume(unsigned short vol);
unsigned short getVolume(void);

void setVolumeUp(void);
void setVolumeDown(void);

void ghostSeek(void);