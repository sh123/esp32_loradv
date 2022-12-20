#ifndef LORADV_SERVICE_H
#define LORADV_SERVICE_H

#include <Arduino.h>
#include <memory>
#include <DebugLog.h>
#include <RadioLib.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <AiEsp32RotaryEncoder.h>
#include <driver/i2s.h>
#include <codec2.h>
#include <CircularBuffer.h>
#include <arduino-timer.h>

#if __has_include("/tmp/esp32_loradv_config.h")
#pragma message("Using external config")
#include "/tmp/esp32_loradv_config.h"
#else
#pragma message("Using default built-in config")
#include "config.h"
#endif

#include "loradv_config.h"
#include "radio_task.h"

namespace LoraDv {

class Service {

public:
  Service();

  void setup(const Config &conf);
  void loop();

private:
  const uint32_t CfgAudioSampleRate = 8000;       // audio sample rate
  const i2s_port_t CfgAudioI2sSpkId = I2S_NUM_0;  // audio i2s speaker number
  const i2s_port_t CfgAudioI2sMicId = I2S_NUM_1;  // audio i2s mic number
  const int CfgAudioMaxVolume = 100;              // maximum volume value

  const uint32_t CfgAudioPlayBit = 0x01;          // task bit for playback
  const uint32_t CfgAudioRecBit = 0x02;           // task bit for recording

  const int CfgDisplayWidth = 128;                // display width
  const int CfgDisplayHeight = 32;                // display height

  const int CfgAudioTaskStack = 32768;

private:
  void setupAudio(int bytesPerSample);

  static IRAM_ATTR void isrReadEncoder();

  float getBatteryVoltage();
  void printStatus(const String &str);

  static bool lightSleepEnterTimer(void *param);
  void lightSleepReset();
  void lightSleepEnter(void);
  esp_sleep_wakeup_cause_t lightSleepWait(uint64_t sleepTimeUs);

  static void audioTask(void *param);
  void audioPlayRecord();

private:
  Config config_;

  // peripherals
  RadioTask radioTask_;
  std::shared_ptr<Adafruit_SSD1306> display_;
  static std::shared_ptr<AiEsp32RotaryEncoder> rotaryEncoder_;

  // tasks
  TaskHandle_t audioTaskHandle_;

  // timers
  Timer<1> lightSleepTimer_;
  Timer<1>::Task lightSleepTimerTask_;

  // other
  volatile bool btnPressed_;
  long codecVolume_;
  int codecBytesPerFrame_;

}; // Service

} // LoraDv

#endif // LORADV_SERVICE_H