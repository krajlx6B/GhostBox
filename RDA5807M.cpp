#include <Arduino.h>
#include <Wire.h>
#include <TeensyThreads.h>

#include "RDA5807M.h"

// ----- Register Definitions -----

// this chip only supports FM mode
#define FREQ_STEPS 10

#define RADIO_REG_CHIPID 0x00

#define RADIO_REG_CTRL 0x02
#define RADIO_REG_CTRL_OUTPUT 0x8000
#define RADIO_REG_CTRL_UNMUTE 0x4000
#define RADIO_REG_CTRL_MONO 0x2000
#define RADIO_REG_CTRL_BASS 0x1000
#define RADIO_REG_CTRL_SEEKUP 0x0200
#define RADIO_REG_CTRL_SEEK 0x0100
#define RADIO_REG_CTRL_RDS 0x0008
#define RADIO_REG_CTRL_NEW 0x0004
#define RADIO_REG_CTRL_RESET 0x0002
#define RADIO_REG_CTRL_ENABLE 0x0001

#define RADIO_REG_CHAN 0x03
#define RADIO_REG_CHAN_SPACE 0x0003
#define RADIO_REG_CHAN_SPACE_100 0x0000
#define RADIO_REG_CHAN_BAND 0x000C
#define RADIO_REG_CHAN_BAND_FM 0x0000
#define RADIO_REG_CHAN_BAND_FMWORLD 0x0008
#define RADIO_REG_CHAN_TUNE 0x0010
//      RADIO_REG_CHAN_TEST   0x0020
#define RADIO_REG_CHAN_NR 0x7FC0

#define RADIO_REG_R4 0x04
#define RADIO_REG_R4_EM50 0x0800
//      RADIO_REG_R4_RES   0x0400
#define RADIO_REG_R4_SOFTMUTE 0x0200
#define RADIO_REG_R4_AFC 0x0100


#define RADIO_REG_VOL 0x05
#define RADIO_REG_VOL_VOL 0x000F


#define RADIO_REG_RA 0x0A
#define RADIO_REG_RA_RDS 0x8000
#define RADIO_REG_RA_RDSBLOCK 0x0800
#define RADIO_REG_RA_STEREO 0x0400
#define RADIO_REG_RA_NR 0x03FF

#define RADIO_REG_RB 0x0B
#define RADIO_REG_RB_FMTRUE 0x0100
#define RADIO_REG_RB_FMREADY 0x0080


#define RADIO_REG_RDSA 0x0C
#define RADIO_REG_RDSB 0x0D
#define RADIO_REG_RDSC 0x0E
#define RADIO_REG_RDSD 0x0F

// I2C-Address RDA Chip for sequential  Access
#define I2C_SEQ 0x10

// I2C-Address RDA Chip for Index  Access
#define I2C_INDX 0x11

#define RADIO_I2CADDRESS 0x03



// Constructor
RDA5807M::RDA5807M() {
    _maxVolume = 15;
}

void RDA5807M::setup(int feature, int value) {
  //if (feature == RADIO_RESETPIN) {
  //  _resetPin = value;
  //} else 
  if ((feature == RADIO_I2CADDRESS) && (value > 0)) {
    _i2caddr = value;
  } else if ((feature == RADIO_ANTENNA) && (value > 0)) {
    _antennaOption = value;
  } else if ((feature == RADIO_FMSPACING) && (value > 0)) {
    _fmSpacing = value;
  } else if ((feature == RADIO_DEEMPHASIS) && (value > 0)) {
    _deEmphasis = value;
  }
}

// initialize Wire lib functions
bool RDA5807M::initWire(TwoWire &port) {
  DEBUG_FUNC0("RADIO::initWire");
  _i2cPort = &port;
  return (this->init());
}

// initialize all internals.
bool RDA5807M::init() {
  bool result = false;  // no chip found yet.
  DEBUG_FUNC0("init");
  if (_resetPin >= 0) {
    // create a reset impulse
    pinMode(_resetPin, OUTPUT);
    digitalWrite(_resetPin, LOW);  // Put chip into reset
    delay(5);
    digitalWrite(_resetPin, HIGH);  // Bring chip out of reset
    delay(5);
  }
  _i2cPort->begin();
  _i2cPort->beginTransmission(I2C_INDX);
  result = _i2cPort->endTransmission();

  if (result != 0) {
    DEBUG_STR("NO radio found.");
  } else {
    DEBUG_STR("radio found.");
    result = true;
    // initialize all registers
    registers[RADIO_REG_CHIPID] = 0x5804;  // 00 id
    registers[1] = 0x0000;                 // 01 not used
    registers[RADIO_REG_CTRL] = (RADIO_REG_CTRL_RESET | RADIO_REG_CTRL_ENABLE);
    setBand(RADIO_BAND_FM);
    registers[RADIO_REG_R4] = RADIO_REG_R4_EM50;  //  0x1800;  // 04 DE ? SOFTMUTE
    registers[RADIO_REG_VOL] = 0x9081;            // 0x81D1;  // 0x82D1 / INT_MODE, SEEKTH=0110,????, Volume=1
    registers[6] = 0x0000;
    registers[7] = 0x0000;
    registers[8] = 0x0000;
    registers[9] = 0x0000;
    // reset the chip
    _saveRegisters();
    registers[RADIO_REG_CTRL] = RADIO_REG_CTRL_ENABLE;
    _saveRegister(RADIO_REG_CTRL);
  } 
  return (result);
} 

// switch the power off
void RDA5807M::term() {
  DEBUG_FUNC0("term");
  setVolume(0);
  registers[RADIO_REG_CTRL] = 0x0000;  // all bits off
  _saveRegisters();
}

// audio and tunning
bool RDA5807M::getghostMode() {
  return(_ghostMode);
}

void RDA5807M::setghostMode(bool radioMode) {
  _ghostMode = radioMode;
}

void RDA5807M::ghostSeek() {
  uint16_t currentFreq = RDA5807M::getFrequency();
  currentFreq = currentFreq + 10;
  if(currentFreq > 10800) {
      currentFreq = 8750;
  }
  RDA5807M::setFrequency(currentFreq);
  delay(seekDelay);  
}

void RDA5807M::ghostDelay() {
  seekDelay = seekDelay + 50;
    if(seekDelay > 300) seekDelay = 50;
}

int RDA5807M::getseekDelay() {
  return(seekDelay);
}

int8_t RDA5807M::getMaxVolume() {
  return (_maxVolume);
} 

void RDA5807M::setVolume(int8_t newVolume) {
  DEBUG_FUNC1("setVolume", newVolume);
    _volume = constrain(newVolume, 0, _maxVolume);
  uint8_t v = _volume & RADIO_REG_VOL_VOL;
  registers[RADIO_REG_VOL] &= (~RADIO_REG_VOL_VOL);
  registers[RADIO_REG_VOL] |= v;
  _saveRegister(RADIO_REG_VOL);
}  // setVolume()

void RDA5807M::setBassBoost(bool switchOn) {
  _bassBoost = switchOn;
  uint16_t regCtrl = registers[RADIO_REG_CTRL];
  if (switchOn)
    regCtrl |= RADIO_REG_CTRL_BASS;
  else
    regCtrl &= (~RADIO_REG_CTRL_BASS);
  registers[RADIO_REG_CTRL] = regCtrl;
  _saveRegister(RADIO_REG_CTRL);
}

void RDA5807M::setMono(bool switchOn) {
  _mono = switchOn;
  registers[RADIO_REG_CTRL] &= (~RADIO_REG_CTRL_SEEK);
  if (switchOn) {
    registers[RADIO_REG_CTRL] |= RADIO_REG_CTRL_MONO;
  } else {
    registers[RADIO_REG_CTRL] &= ~RADIO_REG_CTRL_MONO;
  }
  _saveRegister(RADIO_REG_CTRL);
}

void RDA5807M::setMute(bool switchOn) {
  _mute = switchOn;
  if (switchOn) {
    // now mute
    registers[RADIO_REG_CTRL] &= (~RADIO_REG_CTRL_UNMUTE);
  } else {
    // now unmute
    registers[RADIO_REG_CTRL] |= RADIO_REG_CTRL_UNMUTE;
  } 
  _saveRegister(RADIO_REG_CTRL);
}

int8_t RDA5807M::getVolume() {
  return (_volume);
}

/// The base implementation returns only the value in the internal variable.
bool RDA5807M::getMute() {
  return (_mute);
}

/// The base implementation returns only the value in the internal variable.
bool RDA5807M::getSoftMute() {
  return (_softMute);
}

void RDA5807M::setSoftMute(bool switchOn) {
  _softMute = switchOn;
  if (switchOn) {
    registers[RADIO_REG_R4] |= (RADIO_REG_R4_SOFTMUTE);
  } else {
    registers[RADIO_REG_R4] &= (~RADIO_REG_R4_SOFTMUTE);
  }
  _saveRegister(RADIO_REG_R4);
}

bool RDA5807M::getBassBoost() {
  return (_bassBoost);
}  // getBassBoost()

bool RDA5807M::getMono() {
  return (_mono);
}  // getMono()

void RDA5807M::setBand(RADIO_BAND newBand) {
  uint16_t r;
  _band = newBand;
  if (newBand == RADIO_BAND_FM) {
    _freqLow = 8700;
    _freqHigh = 10800;
    _freqSteps = 10;  // 20 in USA ???

  } else if (newBand == RADIO_BAND_FMWORLD) {
    _freqLow = 7600;
    _freqHigh = 10800;
    _freqSteps = 10;
  }
  if (newBand == RADIO_BAND_FM) {
    r = RADIO_REG_CHAN_BAND_FM;
  } else if (newBand == RADIO_BAND_FMWORLD) {
    r = RADIO_REG_CHAN_BAND_FMWORLD;
  } else {
    return;
  }
  registers[RADIO_REG_CHAN] = (r | RADIO_REG_CHAN_SPACE_100);
  _saveRegister(RADIO_REG_CHAN);
} 

RADIO_FREQ RDA5807M::getFrequency() {
  // check register A
  _i2cPort->requestFrom(I2C_SEQ, 2);
  registers[RADIO_REG_RA] = _read16HL(_i2cPort);
  uint16_t ch = registers[RADIO_REG_RA] & RADIO_REG_RA_NR;
  _freq = _freqLow + (ch * 10);  // assume 100 kHz spacing
  return (_freq);
}

void RDA5807M::setFrequency(RADIO_FREQ newF) {
  DEBUG_FUNC1("setFrequency", newF);
  uint16_t newChannel;
  uint16_t regChannel = registers[RADIO_REG_CHAN] & (RADIO_REG_CHAN_SPACE | RADIO_REG_CHAN_BAND);
  if (newF < _freqLow) newF = _freqLow;
  if (newF > _freqHigh) newF = _freqHigh;
  newChannel = (newF - _freqLow) / 10;
  regChannel += RADIO_REG_CHAN_TUNE;  // enable tuning
  regChannel |= newChannel << 6;
  // enable output and unmute
  registers[RADIO_REG_CTRL] |= RADIO_REG_CTRL_OUTPUT | RADIO_REG_CTRL_UNMUTE | RADIO_REG_CTRL_RDS | RADIO_REG_CTRL_ENABLE;  //  | RADIO_REG_CTRL_NEW
  _saveRegister(RADIO_REG_CTRL);
  registers[RADIO_REG_CHAN] = regChannel;
  _saveRegister(RADIO_REG_CHAN);
  // adjust Volume
  _saveRegister(RADIO_REG_VOL);
}

void RDA5807M::seekUp(bool toNextSender) {
  // start seek mode
  registers[RADIO_REG_CTRL] |= RADIO_REG_CTRL_SEEKUP;
  registers[RADIO_REG_CTRL] |= RADIO_REG_CTRL_SEEK;
  _saveRegister(RADIO_REG_CTRL);

  registers[RADIO_REG_CTRL] &= (~RADIO_REG_CTRL_SEEK);  // clear seekmode
  if (!toNextSender) {
    // stop scanning right now
    // registers[RADIO_REG_CTRL] &= (~RADIO_REG_CTRL_SEEK);
    _saveRegister(RADIO_REG_CTRL);
  }
}

void RDA5807M::seekDown(bool toNextSender) {
  registers[RADIO_REG_CTRL] &= (~RADIO_REG_CTRL_SEEKUP);
  registers[RADIO_REG_CTRL] |= RADIO_REG_CTRL_SEEK;
  _saveRegister(RADIO_REG_CTRL);

  registers[RADIO_REG_CTRL] &= (~RADIO_REG_CTRL_SEEK);  // clear seekmode
  if (!toNextSender) {
    // stop scanning right now
    _saveRegister(RADIO_REG_CTRL);
  }
}

// Load all status registers from to the chip
// registers 0A through 0F
// using the sequential read access mode.
void RDA5807M::_readRegisters() {
  _i2cPort->requestFrom(I2C_SEQ, (6 * 2));
  for (int i = 0; i < 6; i++) {
    registers[0xA + i] = _read16HL(_i2cPort);
  }
}

// Save writable registers back to the chip
// The registers 02 through 06, containing the configuration
// using the sequential write access mode.
void RDA5807M::_saveRegisters() {
  DEBUG_FUNC0("saveRegisters");
  _i2cPort->beginTransmission(I2C_SEQ);
  for (int i = 2; i <= 6; i++)
    _write16HL(_i2cPort, registers[i]);
  _i2cPort->endTransmission();
}

// Save one register back to the chip
void RDA5807M::_saveRegister(byte regNr) {
  DEBUG_FUNC2X("saveRegister", regNr, registers[regNr]);
  _i2cPort->beginTransmission(I2C_INDX);
  _i2cPort->write(regNr);
  _write16HL(_i2cPort, registers[regNr]);
  _i2cPort->endTransmission();
}

/// Retrieve all the information related to the current radio receiving situation.
void RDA5807M::getRadioInfo(RADIO_INFO *info) {
  // set everything to false and 0.
  memset(info, 0, sizeof(RADIO_INFO));
  // info->tuned = false;
  // info->rds = false;
  // info->stereo = false;
  // use current settings
  info->mono = _mono;
  // read data from registers A .. F of the chip into class memory
  _readRegisters();
  info->active = true;  // ???
  if (registers[RADIO_REG_RA] & RADIO_REG_RA_STEREO) info->stereo = true;
  if (registers[RADIO_REG_RA] & RADIO_REG_RA_RDS) info->rds = true;
  info->rssi = registers[RADIO_REG_RB] >> 10;
  if (registers[RADIO_REG_RB] & RADIO_REG_RB_FMTRUE) info->tuned = true;
  if (registers[RADIO_REG_CTRL] & RADIO_REG_CTRL_MONO) info->mono = true;
} 

// ----- Debug functions -----
bool RDA5807M::_wireDebugFlag = false;

void RDA5807M::_wireDebug(bool enable) {
  _wireDebugEnabled = enable;
  _wireDebugFlag = enable;
}

void RDA5807M::debugEnable(bool enable) {
  _debugEnabled = enable;
}

void RDA5807M::debugScan() {
  DEBUG_FUNC0("debugScan");
  uint16_t regChannel = registers[RADIO_REG_CHAN] & (RADIO_REG_CHAN_SPACE | RADIO_REG_CHAN_BAND);
  RADIO_FREQ f = _freqLow;
  int channel = 0;
  while (f < _freqHigh) {
    registers[RADIO_REG_CHAN] = regChannel | RADIO_REG_CHAN_TUNE | (channel << 6);
    _saveRegister(RADIO_REG_CHAN);
    delay(500);
    debugStatus();
    f += _freqSteps;
    channel += 1;
  } 
} 

// send a status report to the serial port
// dump all registers to Serial output
void RDA5807M::debugStatus() {
  char s[12];
  // read data from registers A .. F of the chip into class memory
  _readRegisters();
  formatFrequency(s, sizeof(s));
  Serial.print("Frequency=");
  Serial.print(s);
  uint16_t pi = registers[RADIO_REG_RDSA];
  Serial.print(" PI=");
  _printHex4(pi);
  Serial.print((registers[RADIO_REG_RA] & RADIO_REG_RA_STEREO) ? " Stereo" : " Mono  ");
  Serial.print((registers[RADIO_REG_RA] & RADIO_REG_RA_RDS) ? " ---" : " RDS");
  int rssi = registers[RADIO_REG_RB] >> 10;
  Serial.print(" Sig=");
  if (rssi < 10) Serial.write(' ');
  Serial.print(rssi);
  Serial.print(' ');
  for (int i = 0; i < rssi - 15; i++) { Serial.write('*'); }  // Empfangspegel ab 15. Zeichen
  Serial.println();
  // ruler
  Serial.println("0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F");
  // variables
  for (int n = 0; n < 16; n++) { _printHex4(registers[n]); }
  Serial.println();
  // registers
  _i2cPort->beginTransmission(I2C_INDX);   // Device 0x11 for random access
  _i2cPort->write(0x00);                   // Start at Register 0x0C
  _i2cPort->endTransmission(0);            // restart condition
  _i2cPort->requestFrom(I2C_INDX, 32, 1);  // Retransmit device address with READ, followed by 8 bytes
  for (int n = 0; n < 16; n++) {
    _printHex4(_read16HL(_i2cPort));
  }
  Serial.println();

  // clear text information in Registers
  if (getBassBoost()) Serial.print("BassBoost ");
  if (getMono()) Serial.print("Mono ");
  if (getSoftMute()) Serial.print("Soft mute ");
  int v = getVolume();
  Serial.print("Volume=");
  Serial.print(v);
  Serial.print(' ');
  Serial.println();
}

void RDA5807M::setBandFrequency(RADIO_BAND newBand, RADIO_FREQ newFreq) {
  setBand(newBand);
  setFrequency(newFreq);
} 

RADIO_BAND RDA5807M::getBand() {
  return (_band);
}

// read a 16 bit value in High-Low order from the Wire.
uint16_t RDA5807M::_read16HL(TwoWire *port) {
  uint8_t hiByte = port->read();
  uint8_t loByte = port->read();
  return ((hiByte << 8) + loByte);
}  // _read16HL

// format the current frequency for display and printing
void RDA5807M::formatFrequency(char *s, uint8_t length) {
  RADIO_BAND b = getBand();
  RADIO_FREQ f = getFrequency();
  if ((s != NULL) && (length > 10)) {
    *s = '\0';
    if ((b == RADIO_BAND_FM) || (b == RADIO_BAND_FMWORLD)) {
      // " ff.ff MHz" or "fff.ff MHz"
      int16_to_s(s, (uint16_t)f);
      // insert decimal point
      s[5] = s[4];
      s[4] = s[3];
      s[3] = '.';
      // append units
      strcpy(s + 6, " MHz");
    }

    //     f = _freqLow + (channel * _bandSteps);
    //     if (f < 10000) Serial.write(' ');
    //     Serial.print(f / 100); Serial.print('.'); Serial.print(f % 100); Serial.print(" MHz ");
  }
}

// write a 16 bit value in High-Low order to the Wire.
void RDA5807M::_write16HL(TwoWire *port, uint16_t val) {
  port->write(val >> 8);
  port->write(val & 0xFF);
} 

/// This is a special format routine used to format frequencies as strings with leading blanks.
/// up to 5 digits only ("    0".."99999")
/// *s MUST be able to hold the characters
void RDA5807M::int16_to_s(char *s, uint16_t val) {
  uint8_t n = 5;
  while (n > 0) {
    n--;
    if ((n == 4) || (val > 0)) {
      s[n] = '0' + (val % 10);
      val = val / 10;
    } else {
      s[n] = ' ';
    }
  }
}

/// Prints a word as 4 character hexadecimal code with leading zeros.
void RDA5807M::_printHex4(uint16_t val) {
  Serial.print(' ');
  if (val <= 0x000F)
    Serial.print('0');  // if less 2 Digit
  if (val <= 0x00FF)
    Serial.print('0');  // if less 3 Digit
  if (val <= 0x0FFF)
    Serial.print('0');  // if less 4 Digit
  Serial.print(val, HEX);
}
