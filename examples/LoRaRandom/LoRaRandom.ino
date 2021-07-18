#include <SPI.h>
#include <LoRa.h>

#define LRFRQNCY 866E6

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Start LoRaRandom");

  if (!LoRa.begin(LRFRQNCY)) {
    Serial.println("Starting LoRa failed!");
    while(1);
  }
  Serial.println("LoRa init succeeded.");
}

#define NRANDOM 1024

void loop() {
  uint8_t buffer[NRANDOM];
  
  LoRa.random(buffer, NRANDOM);

  // Just print them out
  for(size_t i=0; i<NRANDOM; i++){
    if(!(i%10)){
      Serial.println();
      Serial.print(i);
      Serial.print(":\t");
    }
    Serial.print(buffer[i]);
    Serial.print("\t");
  }
  Serial.println();
}

