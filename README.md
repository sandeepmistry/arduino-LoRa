# This Fork

adds support to **real random numbers** to this library.

The new functions are
 ``` c
   byte rssi_wideband(); // RSSI wideband meassurement - the original "random" fuction
   byte random();
   void random(uint8_t *buffer, size_t size);
 ```
There is some overhead in creating random numbers. So if you need more then one you should use `random(byte *, size_t)`.

The new random functions takes care about send and receive during a call. They wait until a processed packet is fully send and switches of any IRQs during collecting randomness. Before returning all parameters are set to the original state. 

A long time ago I suggested change request [#496](https://github.com/sandeepmistry/arduino-LoRa/pull/496) but until now nothing happend.

If you need not only a palmful random numbers you can have a look at library [LoRandom](https://github.com/Kongduino/LoRandom).

## How is it done?
It first renames the original function `byte random()` to `byte rssi_wideband()` as this is what this function does! It depends on the location of the meassurement and gives only random values from 0 to MAX with MAX<<255. The result (see [here](https://github.com/plybrd/arduino-LoRa/blob/master/doc-random/random-widebandRSSI.png)) looks strongly biased. In my case only numbers between 0 and 31 are returned!

There is an description how to generate random numbers in Application Note AN1200.24 from Semtech.  See Chapter 4 [Random Number Generation for Cryptography](https://semtech.my.salesforce.com/sfc/p/#E0000000JelG/a/440000001NAw/7YN8ZamV70_xR.vPDAAAshm.0Wt4jmRX0nOKkOzQqiI).

To generate an N bit random number, perform N read operation of the register RegRssiWideband (address 0x2c) and use the LSB of the fetched value. The value from RegRssiWideband is derived from a wideband (4MHz) signal strength at the receiver input and the LSB of this value constantly and randomly changes.

Doing so gives this [result](https://github.com/plybrd/arduino-LoRa/blob/master/doc-random/random-asAN1200.24.png). It looks like bit '1' is prefered in comparion to bit '0'. So binary numbers with many '1' occure more often. But we get the whole range of possible numbers.

Last, we add a basic von Neumann extractor which  produce a uniform output even if the distribution of input bits is not uniform so long as each bit has the same probability of being one and there is no correlation between successive bits (see [Bernoulli_sequence@wikipedia](https://en.wikipedia.org/wiki/Bernoulli_process#Bernoulli_sequence) and [Randomness_extractor@wikipedia](https://en.wikipedia.org/wiki/Randomness_extractor)). The [result](https://github.com/plybrd/arduino-LoRa/blob/master/doc-random/random-asAN1200.24-Neumann.png) shows random numbers without bias. The function reproduces the mean value when we pull enough numbers.

**Using the program [ENT](http://www.fourmilab.ch/random/) from fourmilab gives**

### For the new random function of this library
 - Entropy = 7.999980 bits per byte.
 - Optimum compression would reduce the size of this 8757184 byte file by 0 percent.
 - Chi square distribution for 8757184 samples is 247.56, and randomly would exceed this value 61.91 percent of the times.
 - Arithmetic mean value of data bytes is 127.5110 (127.5 = random).
 - Monte Carlo value for Pi is 3.142532185 (error 0.03 percent).
 - Serial correlation coefficient is 0.000407 (totally uncorrelated = 0.0).

### For rssi_wideband() ( sandeepmistry's "random" function)
- Entropy = 3.910721 bits per byte.
 - Optimum compression would reduce the size of this 11800167 byte file by 51 percent.
 - Chi square distribution for 11800167 samples is 311806630.27, and randomly would exceed this value less than 0.01 percent of the times.
 - Arithmetic mean value of data bytes is 14.9272 (127.5 = random).
 - Monte Carlo value for Pi is 4.000000000 (error 27.32 percent).
 - Serial correlation coefficient is 0.000269 (totally uncorrelated = 0.0).


# Arduino LoRa

[![Build Status](https://travis-ci.org/sandeepmistry/arduino-LoRa.svg?branch=master)](https://travis-ci.org/sandeepmistry/arduino-LoRa)

An [Arduino](https://arduino.cc/) library for sending and receiving data using [LoRa](https://www.lora-alliance.org/) radios.

## Compatible Hardware

 * [Semtech SX1276/77/78/79](https://www.semtech.com/apps/product.php?pn=SX1276) based boards including:
   * [Dragino Lora Shield](https://www.dragino.com/products/lora/item/102-lora-shield.html)
   * [HopeRF](https://www.hoperf.com/modules/lora/index.html) [RFM95W](https://www.hoperf.com/modules/lora/RFM95.html), [RFM96W](https://www.hoperf.com/modules/lora/RFM96.html), and [RFM98W](https://www.hoperf.com/modules/lora/RFM98.html)
   * [Modtronix](http://modtronix.com/) [inAir4](http://modtronix.com/inair4.html), [inAir9](http://modtronix.com/inair9.html), and [inAir9B](http://modtronix.com/inair9b.html)
 * [Arduino MKR WAN 1300](https://store.arduino.cc/usa/mkr-wan-1300)
   * **NOTE:** Requires firmware v1.1.6 or later on the on-board Murata module. Please use the [MKRWANFWUpdate_standalone example](https://github.com/arduino-libraries/MKRWAN/blob/master/examples/MKRWANFWUpdate_standalone/MKRWANFWUpdate_standalone.ino) from latest [MKRWAN library](https://github.com/arduino-libraries/MKRWAN) release to update the firmware.
   * **WARNING**: [LoRa.onReceive(...)](https://github.com/sandeepmistry/arduino-LoRa/blob/master/API.md#register-callback) and [LoRa.recieve()](https://github.com/sandeepmistry/arduino-LoRa/blob/master/API.md#receive-mode) is not compatible with this board!

### Semtech SX1276/77/78/79 wiring

| Semtech SX1276/77/78/79 | Arduino |
| :---------------------: | :------:|
| VCC | 3.3V |
| GND | GND |
| SCK | SCK |
| MISO | MISO |
| MOSI | MOSI |
| NSS | 10 |
| NRESET | 9 |
| DIO0 | 2 |


`NSS`, `NRESET`, and `DIO0` pins can be changed by using `LoRa.setPins(ss, reset, dio0)`. `DIO0` pin is optional, it is only needed for receive callback mode. If `DIO0` pin is used, it **must** be interrupt capable via [`attachInterrupt(...)`](https://www.arduino.cc/en/Reference/AttachInterrupt).

**NOTES**:
 * Some boards (like the Arduino Nano), cannot supply enough current for the SX127x in TX mode. This will cause lockups when sending, be sure to use an external 3.3V supply that can provide at least 120mA's when using these boards.
 * If your Arduino board operates at 5V, like the Arduino Uno, Leonardo or Mega, you will need to use a level converter for the wiring to the Semtech SX127x module. Most Semtech SX127x breakout boards do not have logic level converters built-in.

## Installation

### Using the Arduino IDE Library Manager

1. Choose `Sketch` -> `Include Library` -> `Manage Libraries...`
2. Type `LoRa` into the search box.
3. Click the row to select the library.
4. Click the `Install` button to install the library.

### Using Git

```sh
cd ~/Documents/Arduino/libraries/
git clone https://github.com/sandeepmistry/arduino-LoRa LoRa
```

## API

See [API.md](API.md).

## Examples

See [examples](examples) folder.

## FAQ

**1) Initilizating the LoRa radio is failing**

Please check the wiring you are using matches what's listed in [Semtech SX1276/77/78/79 wiring](#semtech-sx1276777879-wiring). You can also use `LoRa.setPins(ss, reset, dio0)` to change the default pins used. Some logic level converters cannot operate at 8 MHz, you can call `LoRa.setSPIFrequency(frequency)` to lower the SPI frequency used by the library. Both API's must be called before `LoRa.begin(...)`.

**2) Can other radios see the packets I'm sending?**

Yes, any LoRa radio that are configured with the same radio parameters and in range can see the packets you send.

**3) Is the data I'm sending encrypted?**

No, all data is sent unencrypted. If want your packet data to be encrypted, you must encrypt it before passing it into this library, followed by decrypting on the receiving end.

**4) How does this library differ from LoRaWAN libraries?**

This library exposes the LoRa radio directly, and allows you to send data to any radios in range with same radio parameters. All data is broadcasted and there is no addressing. LoRaWAN builds on top of LoRA, but adds addressing, encryption, and additional layers. It also requires a LoRaWAN gateway and LoRaWAN network and application server.

**5) Does this library honor duty cycles?**

No, you have to manage it by your self.

**6) Which frequencies can I use?**

You can use [this table](https://www.thethingsnetwork.org/wiki/LoRaWAN/Frequencies/By-Country) to lookup the available frequencies by your country. The selectable frequency also depends on your hardware. You can lookup the data sheet or ask your supplier.

Please also notice the frequency dependent duty cycles for legal reasons!

## License

This libary is [licensed](LICENSE) under the [MIT Licence](https://en.wikipedia.org/wiki/MIT_License).
