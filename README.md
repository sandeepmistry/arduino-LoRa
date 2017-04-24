# Arduino LoRa

[![Build Status](https://travis-ci.org/sandeepmistry/arduino-LoRa.svg?branch=master)](https://travis-ci.org/sandeepmistry/arduino-LoRa)

An [Arduino](http://arduino.cc/) library for sending and receiving data using [LoRa](https://www.lora-alliance.org/) radios.

## Compatible Hardware

 * [Semtech SX1276/77/78/79](http://www.semtech.com/apps/product.php?pn=SX1276) based boards including:
   * [Dragino Lora Shield](http://www.dragino.com/products/module/item/102-lora-shield.html)
   * [HopeRF](http://www.hoperf.com/rf_transceiver/lora/) [RFM95W](http://www.hoperf.com/rf_transceiver/lora/RFM95W.html), [RFM96W](http://www.hoperf.com/rf_transceiver/lora/RFM96W.html), and [RFM98W](http://www.hoperf.com/rf_transceiver/lora/RFM98W.html)
   * [Modtronix](http://modtronix.com/) [inAir4](http://modtronix.com/inair4.html), [inAir9](http://modtronix.com/inair9.html), and [inAir9B](http://modtronix.com/inair9b.html)

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

## License

This libary is [licensed](LICENSE) under the [MIT Licence](http://en.wikipedia.org/wiki/MIT_License).
