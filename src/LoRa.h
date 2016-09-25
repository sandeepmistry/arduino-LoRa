#ifndef LORA_H
#define LORA_H

#include <Arduino.h>
#include <SPI.h>

class LoRaClass : public Stream {
public:
  LoRaClass();

  int begin(long frequency);
  void end();

  int beginPacket();
  int endPacket();

  int parsePacket();
  int packetRssi();

  // from Print
  virtual size_t write(uint8_t byte);
  virtual size_t write(const uint8_t *buffer, size_t size);

  // from Stream
  virtual int available();
  virtual int read();
  virtual int peek();
  virtual void flush();

  void setPins(int ss, int reset);

  void dumpRegisters(Stream& out);

private:
  uint8_t readRegister(uint8_t address);
  void writeRegister(uint8_t address, uint8_t value);
  uint8_t singleTransfer(uint8_t address, uint8_t value);

private:
  SPISettings _spiSettings;
  int _ss;
  int _reset;
  int _frequency;
  int _packetIndex;
};

extern LoRaClass LoRa;

#endif
