#include <Arduino.h>
#include <Wire.h>
#include <TeensyThreads.h>

#include "RDA5807M.h"

/// fixed settings
#define FIX_BAND RADIO_BAND_FM  ///< The band that will be tuned by this sketch is FM
#define FIX_STATION 8750        ///< The station that will be tuned by this sketch is 87.50 MHz
#define FIX_VOLUME 1           ///< The volume that will be set by this sketch is level 1

int seekDelay = 50;

RDA5807M radio;  // Create an instance of Class for RDA5807M Chip

void setup() {

  pinMode(A6, INPUT_PULLUP);
  pinMode(A8, INPUT_PULLUP);
  pinMode(A9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  
  delay(3000);

  // open the Serial port
  Serial.begin(115200);
  Serial.println("RDA5807M Radio init...\n");
  delay(200);

  // enable information to the Serial port
  radio.debugEnable(false);
  radio._wireDebug(false);

  // set FM Options for Europe
  radio.setup(RADIO_FMSPACING, RADIO_FMSPACING_100);   // for EUROPE
  radio.setup(RADIO_DEEMPHASIS, RADIO_DEEMPHASIS_50);  // for EUROPE

  // initialize the Radio
  if (!radio.initWire(Wire)) {
    Serial.println("no radio chip found.");
    delay(4000);
    //ESP.restart();
  };

  // set all radio setting to the fixed values
  radio.setBandFrequency(FIX_BAND, FIX_STATION);
  radio.setVolume(FIX_VOLUME);
  radio.setMono(false);
  radio.setBassBoost(true);
  radio.setMute(false);
  radio.setSoftMute(false);
}

void loop() {
  char s[12];
  int volume;
  int idleTime;

/// manage the buttons
  if (!digitalRead(12)) { //Mode radio or ghost
    delay(200);
    if(!radio.getghostMode()) {
      radio.setghostMode(true);
      Serial.print("mode ghost\n");
    } else {
      radio.setghostMode(false);
      Serial.print("mode radio\n");
    }
  }

  if (!digitalRead(A6)) { //Shift seek time delay
    delay(200);
    radio.ghostDelay();
    idleTime = radio.getseekDelay();
    Serial.printf("Delay: %i\n", idleTime);
  }

  if (!digitalRead(A8)) { //Request volume down
    delay(200);
    volume = radio.getVolume() - 1;
    radio.setVolume(volume);
    Serial.printf("Volume: %i\n", volume);
  }

  if (!digitalRead(A9)) { //Request volume up
    delay(200);
    volume = radio.getVolume() + 1;
    radio.setVolume(volume);
    Serial.printf("Volume: %i\n", volume);
  }
  
  if(radio.getghostMode()) {
    radio.ghostSeek();
  } else {
    if (!digitalRead(11)) { //Seek station down
      delay(200);
      Serial.printf("Seek down \n");
      radio.seekDown(true);
      radio.formatFrequency(s, sizeof(s));
    Serial.printf("Station %s \n", s);
    }
    if (!digitalRead(10)) { //Seek station up
      delay(200);
      Serial.printf("Seek up \n");
      radio.seekUp(true);
      radio.formatFrequency(s, sizeof(s));
      Serial.printf("Station %s \n", s);
    }
  }

 

}
