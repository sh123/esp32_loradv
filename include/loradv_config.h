#ifndef LORADV_CONFIG_H
#define LORADV_CONFIG_H

#include <Arduino.h>
#include <Preferences.h>
#include <DebugLog.h>

namespace LoraDv {

class Config {
public:
  const int Version = 1;

  DebugLogLevel LogLevel;    // log level

  // lora modulation parameters
  long LoraFreqRx;      // lora RX frequency, e.g. 433.775e6
  long LoraFreqTx;      // lora TX frequency, e.g. 433.775e6
  long LoraBw;          // lora bandwidth, e.g. 125e3
  int LoraSf;           // lora spreading factor, e.g. 12
  int LoraCodingRate;   // lora coding rate, e.g. 7
  int LoraPower;        // lora power level in dbm, 20
  int LoraSync_;        // lora sync word/packet id, 0x34
  int LoraCrc_;         // lora crc mode, 0 - disabled, 1 - 1 byte, 2 - 2 bytes
  bool LoraExplicit_;   // lora header mode, true - explicit, false - implicit

  // lora hardware pinouts and isr
  byte LoraPinSs_;       // lora ss pin
  byte LoraPinRst_;      // lora rst pin
  byte LoraPinA_;        // (sx127x - dio0, sx126x/sx128x - dio1)
  byte LoraPinB_;        // (sx127x - dio1, sx126x/sx128x - busy)
  byte LoraPinSwitchRx_; // (sx127x - unused, sx126x - RXEN pin number)
  byte LoraPinSwitchTx_; // (sx127x - unused, sx126x - TXEN pin number)

  // rotary encoder
  byte EncoderPinA_;     // Encoder A pin number
  byte EncoderPinB_;     // Encoder B pin number
  byte EncoderPinBtn_;   // Encoder button pin number
  byte EncoderPinVcc_;   // Encoder VCC pin (or -1 if not connected)
  byte EncoderSteps_;    // Encoder number of steps

  // audio params
  int AudioCodec2Mode;   // Audio Codec2 mode
  int AudioMaxPktSize_;  // Aggregated packet maximum size

  // i2s speaker
  byte AudioSpkPinBclk_; // Speaker i2s clk pin
  byte AudioSpkPinLrc_;  // Speaker i2s lrc pin
  byte AudioSpkPinDin_;  // Speaker i2s din pin

  // i2s mic
  byte AudioMicPinSd_;   // Mic i2s sd pin
  byte AudioMicPinWs_;   // Mic i2s ws pin
  byte AudioMicPinSck_;  // Mic i2s sck pin

  // audio state
  int AudioMaxVol_;      // maximum volume
  int AudioVol;          // current volume

  // battery monitor
  byte BatteryMonPin_;  // Battery monitor adc pin
  int BatteryMonCal;    // Battery monitor calibrarion value

  // power management
  int PmLightSleepAfterMs; // Light sleep activation after given ms
  int PmLightSleepDurationMs_; // How long to sleep
  int PmLightSleepAwakeMs_; // How long to be active

  // ptt button
  int PttBtnPin_;            // ptt pin

public:
  Config();
  void Load();
  void Save();

private:
  Preferences prefs_;

}; // Config

} // LoraDv

#endif // LORADV_CONFIG_H