#ifndef LORADV_CONFIG_H
#define LORADV_CONFIG_H

#include <Arduino.h>
#include <DebugLog.h>

namespace LoraDv {

struct Config {
  DebugLogLevel LogLevel;    // log level

  // lora modulation parameters
  long LoraFreqRx;      // lora RX frequency, e.g. 433.775e6
  long LoraFreqTx;      // lora TX frequency, e.g. 433.775e6
  long LoraBw;          // lora bandwidth, e.g. 125e3
  int LoraSf;           // lora spreading factor, e.g. 12
  int LoraCodingRate;   // lora coding rate, e.g. 7
  int LoraPower;        // lora power level in dbm, 20
  int LoraSync;         // lora sync word/packet id, 0x34
  int LoraCrc;          // lora crc mode, 0 - disabled, 1 - 1 byte, 2 - 2 bytes
  bool LoraExplicit;    // lora header mode, true - explicit, false - implicit

  // lora hardware pinouts and isr
  byte LoraPinSs;       // lora ss pin
  byte LoraPinRst;      // lora rst pin
  byte LoraPinA;        // (sx127x - dio0, sx126x/sx128x - dio1)
  byte LoraPinB;        // (sx127x - dio1, sx126x/sx128x - busy)
  byte LoraPinSwitchRx; // (sx127x - unused, sx126x - RXEN pin number)
  byte LoraPinSwitchTx; // (sx127x - unused, sx126x - TXEN pin number)

  // rotary encoder
  byte EncoderPinA;     // Encoder A pin number
  byte EncoderPinB;     // Encoder B pin number
  byte EncoderPinBtn;   // Encoder button pin number
  byte EncoderPinVcc;   // Encoder VCC pin (or -1 if not connected)
  byte EncoderSteps;    // Encoder number of steps

  // audio params
  int AudioCodec2Mode;  // Audio Codec2 mode
  int AudioMaxPktSize;  // Aggregated packet maximum size

  // i2s speaker
  byte AudioSpkPinBclk; // Speaker i2s clk pin
  byte AudioSpkPinLrc;  // Speaker i2s lrc pin
  byte AudioSpkPinDin;  // Speaker i2s din pin

  // i2s mic
  byte AudioMicPinSd;   // Mic i2s sd pin
  byte AudioMicPinWs;   // Mic i2s ws pin
  byte AudioMicPinSck;  // Mic i2s sck pin

  // audio state
  int AudioMaxVol;      // maximum volume
  int AudioVol;         // current volume

  // battery monitor
  byte BatteryMonPin;   // Battery monitor adc pin
  int BatteryMonCal;    // Battery monitor calibrarion value

  // power management
  int PmLightSleepAfterMs; // Light sleep activation after given ms
  int PmLightSleepDurationMs; // How long to sleep
  int PmLightSleepAwakeMs; // How long to be active

  // ptt button
  int PttBtnPin;            // ptt pin
  gpio_num_t PttBtnGpioPin; // gpio pin

}; // Config

} // LoraDv

#endif // LORADV_CONFIG_H