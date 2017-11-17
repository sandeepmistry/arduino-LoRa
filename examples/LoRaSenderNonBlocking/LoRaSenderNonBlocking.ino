#include <SPI.h>
#include <LoRa.h>

int counter = 0;

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;

  Serial.println("LoRa Sender");

  if (!LoRa.begin(915E6))
  {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }
}

void loop()
{
  if (LoRa.isTransmitting())
  {
    delay(100);
    Serial.print('w');
    return;
  }
  Serial.print("\nSending packet: ");
  Serial.println(counter);

  // send packet
  LoRa.beginPacket();
  LoRa.print("hello ");
  LoRa.print(counter);
  LoRa.endPacketasync();

  counter++;
}