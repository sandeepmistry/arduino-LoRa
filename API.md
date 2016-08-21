# LoRa API

## Include Library

```arduino
#include <LoRa.h>
```

## Setup

### Begin

Initialize the library with the specified frequency.

```arduino
LoRa.begin(frequency);
```
 * `frequency` - frequency in Hz (`433E6`, `866E6`, `915E6`)

Returns `1` on success, `0` on failure.

## Set pins

Override the default `NSS` and `NRESET` pins used by the library. **Must** be called before `LoRa.begin()`.

```arduino
LoRa.setPins(ss, reset);
```
 * `ss` new slave select pin to use, defaults to `10`
 * `reset` new reset pin to use, defaults to `9`

### End

Stop the library

```arduino
LoRa.end()
```

## Sending data

### Begin packet

Start the sequence of sending a packet.

```arduino
LoRa.beginPacket();
```

Returns `1` on success, `0` on failure.

### Writing

Write data to the packet. Each packet can contain up to 255 bytes.

```arduino
LoRa.write(byte);

LoRa.write(buffer, length);
```
* `byte` single byte to write to packet

or

* `buffer` data to write to packet
* `length` size of data to write

Returns the number of bytes written.

**Note:** Other Arduino `Print` API's can also be used to write data into the packet

### End packet

End the sequence of sending a packet.

```arduino
LoRa.endPacket()
```

Returns `1` on success, `0` on failure.

## Receiving data

### Parsing packet

Check if a packet has been received.

```arduino
int packetSize = LoRa.parsePacket();
```

Returns the packet size or `0` if no packet was received.

### Packet RSSI

```arduino
int rssi = LoRa.packetRssi();
```

Returns the RSSI of the received packet.

### Available

```arduino
int availableBytes = LoRa.available()
```

Returns number of bytes available for reading.

### Peeking

Peek at the next byte in the packet.

```arduino
byte b = LoRa.peek();
```

Returns the next byte in the packet or `-1` if no bytes available.

### Reading

Read the next byte from the packet.

```arduino
byte b = LoRa.read();
```

Returns the next byte in the packet or `-1` if no bytes available.

**Note:** Other Arduino [`Stream` API's](https://www.arduino.cc/en/Reference/Stream) can also be used to read data from the packet

