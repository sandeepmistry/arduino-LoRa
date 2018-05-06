#include <SPI.h>
#include <LoRa.h>

volatile bool doRead = false; // Flag set by callback to perform read process in main loop
volatile int incomingPacketSize;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Receiver Callback");

  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  // register the receive callback
  LoRa.onReceive(onReceive);

  // put the radio into receive mode
  LoRa.receive();
}

void loop() {
  // If ISR set the flag, perform read operations
  if (doRead) {
    readMessage();
    doRead = false; // Set flag back to false so next read will happen only after next ISR event
  }
}

void readMessage() {
  // received a packet
  Serial.print("Received packet '");

  // read packet
  for (int i = 0; i < incomingPacketSize; i++) {
    Serial.print((char)LoRa.read());
  }

  // print RSSI of packet
  Serial.print("' with RSSI ");
  Serial.println(LoRa.packetRssi());
}

void onReceive(int packetSize) {
  doRead = true;
  incomingPacketSize = packetSize;
}

