# ESP32 Codec2/OPUS DV Handheld Transceiver

## Introduction
This project is hobby/amateur/toy/educational radio ESP32 based LoRa/FSK Codec2/OPUS DV handheld trasceiver a.k.a. walkie-talkie, which is using 1W E22-400M30S (SX1268) radio module (UHF, 70cm band) or optionally 100mW RA-01/RA-02 (SX1278) module. 

It mimics ordinary FM walkie-talkie, does not use any complex protocol for simplicity, just sends audio codec frames with pre-configured radio digital modulation.

Overall components cost for this project is about 15-20$.

Schematics for new board design with integrated components on the board is available under `extras/schematics`. New board design is in alpha stage, not fully tested yet. Pinouts are compatible with first prototype version.

It is based on https://github.com/sh123/esp32_loraprs modem, but uses additional peripherals:
- I2S speaker module MAX98357A + 4ohm 3w speaker (40mm diameter)
- I2S microphone INMP441
- PTT button
- Rotary encoder with push button
- Small OLED display SSD1306 128x32
- Battery voltage monitoring
- Powered from a single commonly avaialble 18650 cell (for example from old laptop battery)
- Uses combined charge + 5v boost controller based on Hotchip HT4928S

Supports next features:
- Supports LoRa and FSK modulation with configurable modulation parameters from settings
- Supports Codec2 (low bit rate) and OPUS (medium/high bit rate) audio codecs, codec could be selected from settings
- Goes into ESP32 light sleep when no activity, so all power consumption is around 30-40mA when in sleep RX, wakes up on new data from radio module or when user starts transmitting, consumes about 90-100mA in active receive and about 700-800mA in full power 1W transmit
- Settings menu on long encoder button click, allows to change frequency and other parameters
- Output power tunable from settings from ~1mW (for ISM toy usage) up to 1W (for amateur radio experiments)
- Experimental privacy option for ISM low power usage (check your country regulations if it is allowed by the ISM band plan before experimenting!)

## Build instructions
- Modify `include/config.h` if needed
- Install platformio
- Build with platformio
- Upload with platformio

## Picture

Two fully 3d printed prototypes were built  

![Device](extras/images/device.png)

List of peripherals used

![Device](extras/images/peripherals.png)

New board design with all components integrated into the board (except battery and charging circuit)

![Device](extras/schematics/images/board.png)


