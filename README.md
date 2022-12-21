# ESP32 LoRa DV Handheld Transceiver 

## Introduction
This project is amateur radio ESP32 based LoRa Codec2 DV trasceiver, which is using 1W E22-400M30S (SX1268) radio module.

It is based on https://github.com/sh123/esp32_loraprs modem, but uses additional peripherals:
- I2S speaker module MAX98357A + 4ohm 3w speaker (40mm diameter)
- I2S microphone INMP441
- PTT button
- Rotary encoder with push button
- Small OLED display SSD1306 128x32
- Battery voltage control

## Build instructions
- Modify `include/config.h` if needed
- Install platformio
- Build with platformio
- Upload with platformio

## Picture
![Device](extras/images/device.png)
