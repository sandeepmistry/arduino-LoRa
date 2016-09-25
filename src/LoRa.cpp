#include <LoRa.h>

// registers
#define REG_FIFO                 0x00
#define REG_OP_MODE              0x01
#define REG_FRF_MSB              0x06
#define REG_FRF_MID              0x07
#define REG_FRF_LSB              0x08
#define REG_PA_CONFIG            0x09
#define REG_FIFO_ADDR_PTR        0x0d
#define REG_FIFO_TX_BASE_ADDR    0x0e
#define REG_FIFO_RX_BASE_ADDR    0x0f
#define REG_FIFO_RX_CURRENT_ADDR 0x10
#define REG_IRQ_FLAGS            0x12
#define REG_RX_NB_BYTES          0x13
#define REG_PKT_RSSI_VALUE       0x1a
#define REG_PAYLOAD_LENGTH       0x22
#define REG_VERSION              0x42

// modes
#define MODE_LONG_RANGE_MODE     0x80
#define MODE_SLEEP               0x00
#define MODE_STDBY               0x01
#define MODE_TX                  0x03
#define MODE_RX_SINGLE           0x06

// PA config
#define PA_BOOST                 0x80

// IRQ masks
#define IRQ_TX_DONE_MASK         0x08
#define IRQ_RX_DONE_MASK         0x40

#define MAX_PKT_LENGTH           255

LoRaClass::LoRaClass() :
  _spiSettings(10E6, MSBFIRST, SPI_MODE0),
  _ss(10), _reset(9),
  _frequency(0),
  _packetIndex(0)
{
}

int LoRaClass::begin(long frequency)
{
  _frequency = frequency;

  // setup pins
  pinMode(_ss, OUTPUT);
  pinMode(_reset, OUTPUT);

  // perform reset
  digitalWrite(_reset, LOW);
  delay(10);
  digitalWrite(_reset, HIGH);
  delay(10);

  // set SS high
  digitalWrite(_ss, HIGH);

  // start SPI
  SPI.begin();

  // check version
  uint8_t version = readRegister(REG_VERSION);
  if (version != 0x12) {
    return 0;
  }

  // put in sleep mode
  writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_SLEEP);

  // set frequency
  uint64_t frf = ((uint64_t)frequency << 19) / 32000000;
  writeRegister(REG_FRF_MSB, (uint8_t)(frf >> 16));
  writeRegister(REG_FRF_MID, (uint8_t)(frf >> 8));
  writeRegister(REG_FRF_LSB, (uint8_t)(frf >> 0));

  // set base addresses
  writeRegister(REG_FIFO_TX_BASE_ADDR, 0);
  writeRegister(REG_FIFO_RX_BASE_ADDR, 0);

  // set output power to 17 dBm
  writeRegister(REG_PA_CONFIG, PA_BOOST | 0x0f);

  // put in standby mode
  writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);

  return 1;
}

void LoRaClass::end()
{
  // put in sleep mode
  writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_SLEEP);

  // stop SPI
  SPI.end();
}

int LoRaClass::beginPacket()
{
  // put in standby mode
  writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);

  // reset FIFO address and paload length
  writeRegister(REG_FIFO_ADDR_PTR, 0);
  writeRegister(REG_PAYLOAD_LENGTH, 0);

  return 1;
}

int LoRaClass::endPacket()
{
  // put in TX mode
  writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_TX);

  // wait for TX done
  while((readRegister(REG_IRQ_FLAGS) & IRQ_TX_DONE_MASK) == 0);

  // clear IRQ's
  writeRegister(REG_IRQ_FLAGS, IRQ_TX_DONE_MASK);

  return 1;
}

int LoRaClass::parsePacket()
{
  int packetLength = 0;
  int irqFlags = readRegister(REG_IRQ_FLAGS);

  // clear IRQ's
  writeRegister(REG_IRQ_FLAGS, irqFlags);

  if (irqFlags & IRQ_RX_DONE_MASK) {
    // received a packet
    _packetIndex = 0;

    // read packet length
    packetLength = readRegister(REG_RX_NB_BYTES);

    // set FIFO address to current RX address
    writeRegister(REG_FIFO_ADDR_PTR, readRegister(REG_FIFO_RX_CURRENT_ADDR));

    // put in standby mode
    writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
  } else if (readRegister(REG_OP_MODE) != (MODE_LONG_RANGE_MODE | MODE_RX_SINGLE)) {
    // not currently in RX mode

    // reset FIFO address
    writeRegister(REG_FIFO_ADDR_PTR, 0);

    // put in single RX mode
    writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_RX_SINGLE);
  }

  return packetLength;
}

int LoRaClass::packetRssi()
{
  return (readRegister(REG_PKT_RSSI_VALUE) - (_frequency < 868E6 ? 164 : 157));
}

size_t LoRaClass::write(uint8_t byte)
{
  return write(&byte, sizeof(byte));
}

size_t LoRaClass::write(const uint8_t *buffer, size_t size)
{
  int currentLength = readRegister(REG_PAYLOAD_LENGTH);

  // check size
  if ((currentLength + size) > MAX_PKT_LENGTH) {
    size = MAX_PKT_LENGTH - currentLength;
  }

  // write data
  for (size_t i = 0; i < size; i++) {
    writeRegister(REG_FIFO, buffer[i]);
  }

  // update length
  writeRegister(REG_PAYLOAD_LENGTH, currentLength + size);

  return size;
}

int LoRaClass::available()
{
  return (readRegister(REG_RX_NB_BYTES) - _packetIndex);
}

int LoRaClass::read()
{
  if (!available()) {
    return -1;
  }

  _packetIndex++;

  return readRegister(REG_FIFO);
}

int LoRaClass::peek()
{
  if (!available()) {
    return -1;
  }

  // store current FIFO address
  int currentAddress = readRegister(REG_FIFO_ADDR_PTR);

  // read
  uint8_t b = readRegister(REG_FIFO);

  // restore FIFO address
  writeRegister(REG_FIFO_ADDR_PTR, currentAddress);

  return b;
}

void LoRaClass::flush()
{
}

void LoRaClass::setPins(int ss, int reset)
{
  _ss = ss;
  _reset = reset;
}

void LoRaClass::dumpRegisters(Stream& out)
{
  for (int i = 0; i < 128; i++) {
    out.print("0x");
    out.print(i, HEX);
    out.print(": 0x");
    out.println(readRegister(i), HEX);
  }
}

uint8_t LoRaClass::readRegister(uint8_t address)
{
  return singleTransfer(address & 0x7f, 0x00);
}

void LoRaClass::writeRegister(uint8_t address, uint8_t value)
{
  singleTransfer(address | 0x80, value);
}

uint8_t LoRaClass::singleTransfer(uint8_t address, uint8_t value)
{
  uint8_t response;

  digitalWrite(_ss, LOW);

  SPI.beginTransaction(_spiSettings);
  SPI.transfer(address);
  response = SPI.transfer(value);
  SPI.endTransaction();

  digitalWrite(_ss, HIGH);

  return response;
}

LoRaClass LoRa;
