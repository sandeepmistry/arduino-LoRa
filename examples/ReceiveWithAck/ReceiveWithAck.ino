/*
  ReceiveWithAck

  Send ACK for a message back to the sender if requested.

  The raw message is read into an array. Out of this array the message header
  and the actual message payload (user data) is extracted.
  If the address of this node is the recipient's address the ACK is
  transmitted to the sender of the message.
  This is only done if the message was not a broadcast message and the sender
  requested an ACK from the receiver.

  created 4 Nov 2020
  by brainelectronics
*/

#include <SPI.h>
#include <LoRa.h>

byte localAddress = 0x2;        //< address of this device

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Receiver with ACK");

  if (!LoRa.begin(915E6)) {         // initialize ratio at 915 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                   // if failed, do nothing
  }

  Serial.println("LoRa init succeeded.");
}

void loop() {
  // all magic happens here
  onReceive(LoRa.parsePacket());
}

void onReceive(int packetSize)
{
  // if there's no packet, return
  if (packetSize == 0) return;

  // create different arrays
  uint8_t packet[packetSize];                 // complete received message
  uint8_t rawHeader[HEADER_SIZE];             // header of message

  // read recieved bytes into buffer called 'packet'
  LoRa.readToBuffer(packet);

  // extract header from complete 'packet' into 'rawHeader' buffer
  LoRa.getPacketHeader(packet, rawHeader);

  // get the recipient of this message
  uint8_t recipient = LoRa.getRecipient(rawHeader);

  // compare the recipient with the address of this node
  if (recipient != localAddress)
  {
    // this message is not for this node
    Serial.print("This message is not for this node with ID ");
    Serial.println(localAddress);
    return;
  }

  // extract payload from complete 'packet' into 'rawPayload' buffer
  uint8_t rawPayload[packetSize-HEADER_SIZE]; // payload of message
  LoRa.getPacketMessage(packet, rawPayload, packetSize);

  // print payload of message
  printRawMessageData(rawPayload, packetSize);

  // send an ACK back to the sender
  LoRa.sendAck(rawHeader, localAddress);
}

void printRawMessageData(uint8_t *pPayload, uint8_t packetSize)
{
  Serial.print("Received (raw payload): '");

  for (uint8_t i = 0; i < (packetSize-HEADER_SIZE); i++) {
    Serial.print((char)pPayload[i]);
  }

  // print RSSI of packet
  Serial.print("' with RSSI ");
  Serial.println(LoRa.packetRssi());
}
