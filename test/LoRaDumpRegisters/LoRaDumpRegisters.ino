/*
  LoRa register dump
  context: Arduino

*/
#include <SPI.h>              // include libraries
#include <LoRa.h>

void setup() {
  while (!Serial);
  Serial.begin(9600);               // initialize serial
  Serial.println("LoRa Receiver");
  LoRa.setPins(7, 6, 1);            // set CS, reset, IRQ pin
  if (!LoRa.begin(915E6)) {         // initialize ratio at 915Mhz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                   // if failed, do nothing
  }
  LoRa.sleep();
 LoRa.dumpRegisters(Serial);
}


void loop() {
  
}

