/*
  LoRa Simple Random Seed

  This code generate a randomSeed using the LoRa Module.

  created 03 October 2018
  by Luiz H. Cassettari
*/

#include <SPI.h>              // include libraries
#include <LoRa.h>

const long frequency = 915E6;  // LoRa Frequency

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 9;        // LoRa radio reset
const int irqPin = 2;          // change for your board; must be a hardware interrupt pin

uint32_t seed = 0;

void setup() {
  Serial.begin(9600);                   // initialize serial
  while (!Serial);

  LoRa.setPins(csPin, resetPin, irqPin);

  if (!LoRa.begin(frequency)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }

  Serial.println("LoRa init succeeded.");
  Serial.println();
  Serial.println("LoRa Simple Random");
  Serial.println("'LoRa.random()' only works when the module is on receive mode");
  Serial.println("This code generate a randomSeed using the LoRa Module");
  Serial.println();

  LoRa.receive();

  // Generate a random seed with 32 bits
  seed = (uint32_t) LoRa.random() << 24 |
         (uint32_t) LoRa.random() << 16 |
         (uint32_t) LoRa.random() << 8 |
         (uint32_t) LoRa.random();

  LoRa.sleep();

  randomSeed(seed);
  Serial.print("Random Seed : ");
  Serial.println(seed);
  Serial.println();
}

void loop() {
  for (int i = 0; i < 16; i++) {
    byte b = random();
    if (b < 0x10) 
      Serial.print('0');
    Serial.print(b, HEX);
    Serial.print(" ");
  }
  Serial.println();
  delay(1000);
}