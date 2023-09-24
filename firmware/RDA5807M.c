#include "RDA5807M.h"

unsigned short radioRegisterContent[4];
unsigned char buffer[10];
int volume;
int frequence;

void radioInit(void){
	unsigned char radioAddress;
		
	UartInit();
	
	//Scan for device
	radioAddress = I2C_Scan();
	
	if(radioAddress){
		reset();
		delay_ms(100);
	
		readRegisters();

		PrintSerial("Radio found at 0x%02x\n", radioAddress);
		PrintSerial("Loading defaults...\n");

		radioRegisterContent[0] = 0xD001; 				//Power up and set Audio output, Mute and Bass boost
		radioRegisterContent[2] = RADIO_REG_R4_EM50; 	//Set de emphasis
		writeRegisters();
		setVolume(RADIO_DEFAULT_VOL); 					//Set default volume
		setFreq(RADIO_DEFAULT_FRQ); 					//Set default freq

	} else PrintSerial("Device not found\n" );
}

unsigned short readRegisterRan(unsigned char regAddress){
	unsigned short dat;
	unsigned char highByte;
	unsigned char lowByte;
	
	I2C_Start();
	I2C_WriteByte(RND_WriteAdr);
	I2C_WriteByte(regAddress);
	I2C_Start();
	I2C_WriteByte(RND_ReadAdr);
	highByte = I2C_ReadByte();
	I2C_send_ack();
	lowByte = I2C_ReadByte();
	I2C_send_nack();
	I2C_Stop();

	dat = (highByte << 8) + lowByte;
	
	return dat;
}

void writeRegisterRan(unsigned char regAddress, unsigned short dat){
	unsigned char highByte;
	unsigned char lowByte;
		
	highByte = (dat >>8 ) & 0xFF;
	lowByte = dat & 0xFF;
	
	I2C_Start();
	I2C_WriteByte(RND_WriteAdr);
	I2C_WriteByte(regAddress);
	I2C_WriteByte(highByte);
	I2C_WriteByte(lowByte);
	I2C_Stop();
}

void readRegisters(void){
	radioRegisterContent[0] = readRegisterRan(Reg02Address);
	radioRegisterContent[1] = readRegisterRan(Reg03Address);
	radioRegisterContent[2] = readRegisterRan(Reg04Address);
	radioRegisterContent[3] = readRegisterRan(Reg05Address);	
}

void writeRegisters(void){
	writeRegisterRan(Reg02Address,radioRegisterContent[0]);
	writeRegisterRan(Reg03Address,radioRegisterContent[1]);
	writeRegisterRan(Reg04Address,radioRegisterContent[2]);
	writeRegisterRan(Reg05Address,radioRegisterContent[3]);
	readRegisters();
}

void reset(void){
	radioRegisterContent[0] |= RADIO_REG_CTRL_RESET;
	writeRegisters();
}

void setFreq(unsigned short freq) {
	unsigned short newChan;
	
	newChan = (freq - 8700) / 10;
	newChan = newChan << 6;
	newChan += 0x0010; //Add the tune bit to 1
	radioRegisterContent[1] = newChan;
	writeRegisters();
	PrintSerial("Station %i\n", getFreq());
}

unsigned short getFreq(void) {
	unsigned short chan;
	unsigned short freq;

	readRegisters();

	chan = radioRegisterContent[1];
	chan = chan >> 6;
	freq = chan*10 + 8700;
	return (freq);
}

void seekUp(void) {
	radioRegisterContent[0] |= RADIO_REG_CTRL_SEEKUP;
  	radioRegisterContent[0] |= RADIO_REG_CTRL_SEEK;
	writeRegisters();
	radioRegisterContent[0] &= RDA_REG_SEEK_STOP;
}

void seekDown(void){
	radioRegisterContent[0] &= (~RADIO_REG_CTRL_SEEKUP);
  	radioRegisterContent[0] |= RADIO_REG_CTRL_SEEK;
	writeRegisters();
	radioRegisterContent[0] &= RDA_REG_SEEK_STOP;
}

void setVolume(unsigned short vol){
	radioRegisterContent[3] &=~(RADIO_REG_VOL_VOL);
	radioRegisterContent[3] |= vol;
	writeRegisters();
}

unsigned short getVolume(void) {
	unsigned short vol;

	readRegisters();
	vol = radioRegisterContent[3] & 0x000F;
	return (vol);
}

void setVolumeUp(void) {
	unsigned short vol;

	vol = getVolume();
	if(vol < 16) setVolume(vol +1);
}

void setVolumeDown(void){
	unsigned short vol;

	vol = getVolume();
	if(vol > 0) setVolume(vol -1);
}

void ghostSeek(void) {
	unsigned short freq;
	
	freq = getFreq();
	if(freq == 10800) freq = 8700;
	else freq = freq + 10;
	setFreq(freq);
}
