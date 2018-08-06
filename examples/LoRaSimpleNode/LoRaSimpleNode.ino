/*
  LoRa Simple Gateway/Node Exemple

  This code uses InvertIQ function to create a simple Gateway/Node logic.

  Gateway - Sends messages with InvertIQ(true)
          - Receives messeges with InvertIQ(false)

  Node    - Sends messages with InvertIQ(false)
          - Receives messeges with InvertIQ(true)

  With this arrangement a Gateway never receive messagem from another Gateway 
  and a Node never receive message from another Node.
  Only Gateway to Node and vice versa.

  This code receives messagens and sends a message every second.

  InvertIQ function basically invert the LoRa I and Q signals.

  See the Semtech datasheet, http://www.semtech.com/images/datasheet/sx1276.pdf
  for more on InvertIQ register 0x33.

  created 05 August 2018
  by Luiz H. Cassettari
*/

#include <SPI.h>              // include libraries
#include <LoRa.h>

const long frequency = 915E6;  // LoRa Frequency

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 9;        // LoRa radio reset
const int irqPin = 2;          // change for your board; must be a hardware interrupt pin

void setup() {
  Serial.begin(115200);                   // initialize serial
  while (!Serial);

  LoRa.setPins(csPin, resetPin, irqPin);

  if (!LoRa.begin(frequency)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }

  Serial.println("LoRa init succeeded.");
  Serial.println();
  Serial.println("LoRa Simple Node");
  Serial.println("Only receive messages from gateways");
  Serial.println("Tx: invertIQ = false");
  Serial.println("Rx: invertIQ = true");
  Serial.println();

  LoRa.onReceive(onReceive);
  LoRa_rxMode();
}

void loop() {
  if (runEvery(1000)) { // repeat every 1000 millis

    String message = "HeLoRa World! ";   
    message += "I'm a Node! ";
    message += millis();

    LoRa_sendMessage(message); // send a message

    Serial.println("Send Message!");
  }
}

void LoRa_rxMode(){
  LoRa.invertIQ(true);                  // active invert I and Q signals
  LoRa.receive();                       // set receive mode
}

void LoRa_txMode(){
  LoRa.idle();                          // set standby mode
  LoRa.invertIQ(false);                 // normal mode
}

void LoRa_sendMessage(String message) {
  LoRa_txMode();                        // set tx mode
  LoRa.beginPacket();                   // start packet
  LoRa.print(message);                  // add payload
  LoRa.endPacket();                     // finish packet and send it
  LoRa_rxMode();                        // set rx mode
}

void onReceive(int packetSize) {
  String message = "";
  
  while (LoRa.available()) {
    message += (char)LoRa.read();
  }

  Serial.print("Node Receive: ");
  Serial.println(message);

}

boolean runEvery(unsigned long interval) 
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}

