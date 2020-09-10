/********************************************************************
  LoRandomSeed

  Example demonstrating how to generate random numbers using LoRa.

  created 7 Sep 2020
  by Kongduino
  adapted by Andres Sabas
***********************************************************************/

#include <SPI.h>
#include <LoRa.h>

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.print(F("\n\n\n[SX1276] Initializing ... "));
  LoRa.setPins(SS, RFM_RST, RFM_DIO0);
  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.print("Setting up LoRa ");
  
  LoRa.setTxPower(20, PA_OUTPUT_PA_BOOST_PIN);
  LoRa.setPreambleLength(8);
  LoRa.beginRandom();
  Serial.println("End of setup\n\n");
}

void loop() {
  byte randomBytes[256];
  // We'll build a stock of random bytes for use in code
  uint8_t randomIndex = 0;
  uint16_t i;
  for (i = 0; i < 256; i++) {
    uint8_t x = LoRa.random();
    randomBytes[i] = x;
  }
  randomIndex = 0;
  hexDump(randomBytes, 256);
  delay(2000);
}

void hexDump(unsigned char *buf, uint16_t len) {
  String s = "|", t = "| |";
  Serial.println(F("  |.0 .1 .2 .3 .4 .5 .6 .7 .8 .9 .a .b .c .d .e .f |"));
  Serial.println(F("  +------------------------------------------------+ +----------------+"));
  for (uint16_t i = 0; i < len; i += 16) {
    for (uint8_t j = 0; j < 16; j++) {
      if (i + j >= len) {
        s = s + "   "; t = t + " ";
      } else {
        char c = buf[i + j];
        if (c < 16) s = s + "0";
        s = s + String(c, HEX) + " ";
        if (c < 32 || c > 127) t = t + ".";
        else t = t + (String(c));
      }
    }
    int index = i / 16;
    Serial.print(index, HEX); Serial.write('.');
    Serial.println(s + t + "|");
    s = "|"; t = "| |";
  }
  Serial.println(F("  +------------------------------------------------+ +----------------+"));
}