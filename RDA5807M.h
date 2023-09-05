#ifndef RDA5807M_h
#define RDA5807M_h

#include <Arduino.h>
#include <Wire.h>
#include <TeensyThreads.h>

#define UNUSED __attribute__((unused))

// The DEBUG_xxx Macros enable Information to the Serial port.
// They can be enabled by setting the _debugEnabled variable to true disabled by using the debugEnable function.
// When the code has to be minimized they can be redefined without implementation like:
// #define DEBUG_STR(txt) {}

/// Used for Debugging text information.
#define DEBUG_STR(txt) \
  if (_debugEnabled) { \
    Serial.print('>'); \
    Serial.println(txt); \
  }

/// Used for Debugging function entries without parameters.
#define DEBUG_VAL(label, val) \
  if (_debugEnabled) { \
    Serial.print('>'); \
    Serial.print(label); \
    Serial.print(':'); \
    Serial.println(val); \
  }
#define DEBUG_VALX(label, val) \
  if (_debugEnabled) { \
    Serial.print('>'); \
    Serial.print(label); \
    Serial.print(':'); \
    Serial.println(val, HEX); \
  }

/// Used for Debugging function entries without parameters.
#define DEBUG_FUNC0(fn) \
  if (_debugEnabled) { \
    Serial.print('>'); \
    Serial.print(fn); \
    Serial.println("()"); \
  }

/// Used for Debugging function entries with 1 parameter.
#define DEBUG_FUNC1(fn, p1) \
  if (_debugEnabled) { \
    Serial.print('>'); \
    Serial.print(fn); \
    Serial.print('('); \
    Serial.print(p1); \
    Serial.println(')'); \
  }

/// Used for Debugging function entries with 1 parameters as hex Value.
#define DEBUG_FUNC1X(fn, p1) \
  if (_debugEnabled) { \
    Serial.print('>'); \
    Serial.print(fn); \
    Serial.print("(0x"); \
    Serial.print(p1, HEX); \
    Serial.println(')'); \
  }

/// Used for Debugging function entries with 2 parameters.
#define DEBUG_FUNC2(fn, p1, p2) \
  if (_debugEnabled) { \
    Serial.print('>'); \
    Serial.print(fn); \
    Serial.print('('); \
    Serial.print(p1); \
    Serial.print(", "); \
    Serial.print(p2); \
    Serial.println(')'); \
  }

/// Used for Debugging function entries with 2 parameters and Hex Value.
#define DEBUG_FUNC2X(fn, p1, p2) \
  if (_debugEnabled) { \
    Serial.print('>'); \
    Serial.print(fn); \
    Serial.print("(0x"); \
    Serial.print(p1, HEX); \
    Serial.print(", 0x"); \
    Serial.print(p2, HEX); \
    Serial.println(')'); \
  }

// setup() features and defined values
#define RADIO_RESETPIN 0x01
#define RADIO_MODEPIN 0x02
#define RADIO_I2CADDRESS 0x03

#define RADIO_ANTENNA 0x04
#define RADIO_ANTENNA_DEFAULT 0
#define RADIO_ANTENNA_OPT1 1
#define RADIO_ANTENNA_OPT2 2

// FM channel spacing configuration is supported by some radio chips.
#define RADIO_FMSPACING 0x05
#define RADIO_FMSPACING_25 25
#define RADIO_FMSPACING_50 50
#define RADIO_FMSPACING_100 100  // 100 kHz typically used in Europe / Japan = default
#define RADIO_FMSPACING_200 200  // 200 kHz typically used in US / Australia 

// FM High Frequency de-emphasis 
#define RADIO_DEEMPHASIS 0x06
#define RADIO_DEEMPHASIS_50 50 // 50µs typically used in Europe, Australia, Japan
#define RADIO_DEEMPHASIS_75 75 // 75µs typically used in USA

/// Band datatype.
/// The BANDs a receiver probably can implement.
enum RADIO_BAND {
  RADIO_BAND_NONE = 0,  ///< No band selected.

  RADIO_BAND_FM = 0x01,       ///< FM band 87.5 - 108 MHz (USA, Europe) selected.
  RADIO_BAND_FMWORLD = 0x02,  ///< FM band 76 - 108 MHz (Japan, Worldwide) selected.
  RADIO_BAND_AM = 0x03,       ///< AM band selected.
  RADIO_BAND_KW = 0x04,       ///< KW band selected.

  RADIO_BAND_FMTX = 0x11,  ///< Transmit for FM.
};


/// Frequency data type.
/// Only 16 bits are used for any frequency value (not the real one)
typedef uint16_t RADIO_FREQ;

/// A structure that contains information about the radio features from the chip.
struct RADIO_INFO {
  bool active;   ///< receiving is active.
  uint8_t rssi;  ///< Radio Station Strength Information.
  uint8_t snr;   ///< Signal Noise Ratio.
  bool rds;      ///< RDS information is available.
  bool tuned;    ///< A stable frequency is tuned.
  bool mono;     ///< Mono mode is on.
  bool stereo;   ///< Stereo audio is available
};


/// a structure that contains information about the audio features
struct AUDIO_INFO {
  uint8_t volume;
  bool mute;
  bool softmute;
  bool bassBoost;
};


/// Library to control the RDA5807M radio chip.
class RDA5807M {
public:
  RDA5807M();

  void setup(int feature, int value);  ///< configure board/hardware specific features before init().
  bool initWire(TwoWire &port);        // init with I2C bus
  bool init();
  void term();

  // ----- Audio features -----
  int idleTime;
  bool getghostMode();
  void setghostMode(bool radioMode);
  void ghostSeek();
  void ghostDelay();
  int getseekDelay();
  void setMono(bool switchOn);
  bool getMono();               ///< Retrieve the current mono mode setting.
  int8_t getMaxVolume();             ///< Retrieve the maximum possible output volume.
  void setVolume(int8_t newVolume);  ///< Set the volume output of the radio chip.
  int8_t getVolume();                ///< Retrieve the current output volume.
  void setMute(bool switchOn);  ///< Control the mute mode of the radio chip.
  bool getMute();               ///< Retrieve the current mute mode setting.
  void setSoftMute(bool switchOn);  ///< Control the softmute mode (mute on low signals) of the radio chip.
  bool getSoftMute();               ///< Retrieve the current soft mute mode setting.
  void setBassBoost(bool switchOn);  ///< Control the bass boost mode of the radio chip.
  bool getBassBoost();               ///< Retrieve the current bass boost mode setting.

  // ----- Receiver features -----
  void setBand(RADIO_BAND newBand);
  void setFrequency(RADIO_FREQ newF);
  RADIO_FREQ getFrequency(void);
  void setBandFrequency(RADIO_BAND newBand, RADIO_FREQ newFreq);  ///< Set Band and Frequency in one call.
  RADIO_BAND getBand();              ///< Retrieve the current band setting.
  void formatFrequency(char *s, uint8_t length);
  void seekUp(bool toNextSender = true);    // start seek mode upwards
  void seekDown(bool toNextSender = true);  // start seek mode downwards

  // Debug
  void debugEnable(bool enable = true);
  void debugRadioInfo();  ///< Print out all radio information.
  void debugAudioInfo();  ///< Print out all audio information.
  static bool _wireDebugFlag;
  void debugScan();    // Scan all frequencies and report a status
  void debugStatus();  // DebugInfo about actual chip data available
  void _wireDebug(bool enable = true);
  void getRadioInfo(RADIO_INFO *info);  ///< Retrieve some information about the current radio function of the chip.


  // write a 16 bit value in High-Low order to the Wire.
  static void _write16HL(TwoWire *port, uint16_t val);
  static uint16_t _read16HL(TwoWire *port);

protected:
  // ----- local variables
  uint16_t registers[16];  // memory representation of the registers
  int seekDelay = 100;

  // ----- low level communication to the chip using I2C bus
  void _readRegisters();           // read all status & data registers
  void _saveRegisters();           // Save writable registers back to the chip
  void _saveRegister(byte regNr);  // Save one register back to the chip

  // debug
  bool _debugEnabled = false;      ///< Set by debugEnable() and controls debugging functionality.
  bool _wireDebugEnabled = false;  ///< Set by _wireDebug() and controls i2c data level debugging.

  // audio
  uint8_t _volume = 0;      ///< Last set volume level.
  uint8_t _maxVolume = 15;  ///< maximum of volume supported by the chip.
  bool _bassBoost = false;  ///< Last set bass Boost effect.
  bool _mono = false;       ///< Last set mono effect.
  bool _mute = false;       ///< Last set mute effect.
  bool _softMute = false;   ///< Last set softMute effect.
  bool _ghostMode = false;

  // band
  RADIO_BAND _band;  ///< Last set band.
  RADIO_FREQ _freq;  ///< Last set frequency.
  RADIO_FREQ _freqLow;    ///< Lowest frequency of the current selected band.
  RADIO_FREQ _freqHigh;   ///< Highest frequency of the current selected band.
  RADIO_FREQ _freqSteps;  ///< Resolution of the tuner.
  int _fmSpacing = RADIO_FMSPACING_100;
  int _deEmphasis = RADIO_DEEMPHASIS_50;
  int _antennaOption = 0;


  // utils
  void _printHex2(uint8_t val);   ///< Prints a byte as 2 character hexadecimal code with leading zeros.
  void _printHex4(uint16_t val);  ///< Prints a register as 4 character hexadecimal code with leading zeros.

  // i2c bus communication
  TwoWire *_i2cPort;
  int _i2caddr;

  // extra pins
  int _resetPin = -1;

private:
  void int16_to_s(char *s, uint16_t val);  ///< Converts a int16 number to a string, similar to itoa, but using the format "00000".
};

#endif
