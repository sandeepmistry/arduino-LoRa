/*
  LoRa Duplex communication with callback 
  context: Arduino

  This sketch is for testing transmission between two devices, one an Uno,
  and one not an Uno (a MKRZero, in my case). The #defines are there for that
  reason,and should be removed before making this a real example.

   It sends every half second, and polls continually for new packets.

   There's a block that's commented out in the loop that lets you change the
   sync word on one device, to test sync address filtering.  The SyncWordTest.xslx
   spreadsheet shows some typical results.
*/
#include <SPI.h>              // include libraries
#include <LoRa.h>

#if defined (__AVR_ATmega328P__)
#define irqPin 2
#define addr 0xFD
#define netId 0x18
#define change 1
#else
#define irqPin 1
#define addr 0xFE
#define netId 0x18
#define change 0
#endif

String message;               // outgoing message
byte messageId = 0;           // ID of outgoing message
byte localAddress = addr;     // address of this device
byte destination = 0xFF;      // destination to send to
byte syncWord = netId;         // Sync word (network ID)
long lastSend = 0;

void setup() {
  while (!Serial);
  Serial.begin(9600);               // initialize serial
  Serial.println("LoRa Receiver");
  LoRa.setPins(7, 6, irqPin);            // set CS, reset, IRQ pin
  if (!LoRa.begin(915E6)) {         // initialize ratio at 915Mhz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                   // if failed, do nothing
  }
  LoRa.sleep();
  LoRa.setSyncWord(syncWord);
  LoRa.idle();
   // register the receive callback
  LoRa.onReceive(onReceive);

  // put the radio into receive mode
  LoRa.receive();
  LoRa.setTimeout(10);              // set Stream timeout of 10ms
  Serial.println("LoRa init succeeded.");

  Serial.println("Sync Word:" + String(syncWord, HEX));
  Serial.println("Local Addr:" + String(localAddress, HEX));
  Serial.println();
  Serial.print("Received from:\t");
  Serial.print("Sent to:\t");
  Serial.print("Message ID:\t");
  Serial.print("Message length:\t");
  Serial.print("Message:\t");
  Serial.print("Sender's sync word:\t");
  Serial.print("RSSI:\t");
  Serial.println("Snr:");
}

void loop() {
  if (millis() - lastSend > 500) {
    message = "sender's syncWord:\t";  // send a message; tab automatically
    message += String(syncWord, HEX);  // separates the sync word into its own column
    sendMessage(message);
    lastSend = millis();
  }
  //  Change the sync word once every 10 messages:
  //if(messageId % 10 == 0 && change == 1) {
  //  LoRa.sleep();
  //  LoRa.setSyncWord(syncWord++);
  //  LoRa.idle();
  //}
}

void sendMessage(String outgoing) {
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(messageId);                // add message ID
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  messageId++;                          // increment message ID
}


void onReceive(int packetSize) {
  // read packet header bytes:
  int recipient = LoRa.read();       // recipient address
  byte sender = LoRa.read();          // sender address
  byte latestMessage = LoRa.read();   // incoming msg ID
  byte msgLength = LoRa.read();       // incoming msg length
  String incoming = LoRa.readString();// payload of packet

  if (msgLength != incoming.length()) {   // check length for error
    Serial.println("error: message length does not match length");
  }
  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress && recipient != 0xFF) {
    Serial.println("This message is not for me.");
    return;           // skip rest of loop
  }
  // if message is for this device, or broadcast, print details:
  Serial.print(sender, HEX);
  Serial.print("\t");
  Serial.print(recipient, HEX);
  Serial.print("\t");
  Serial.print(latestMessage);
  Serial.print("\t");
  Serial.print(msgLength);
  Serial.print("\t");
  Serial.print(incoming);
  Serial.print("\t");
  Serial.print(LoRa.packetRssi());  // print RSSI of packet
  Serial.print("\t");
  Serial.print(LoRa.packetSnr());
  Serial.println();
}

