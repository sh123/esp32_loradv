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

#include "loradv_config.h"
#include "radio_task.h"
#include "audio_task.h"
#include "pm_service.h"
#include "hw_monitor.h"

namespace LoraDv {

class Service {

public:
  Service();

  void setup(std::shared_ptr<Config> conf);
  void loop();

private:
  const int CfgDisplayWidth = 128;                // display width
  const int CfgDisplayHeight = 32;                // display height

private:
  static IRAM_ATTR void isrReadEncoder();

  void printStatus(const String &str);

private:
  std::shared_ptr<Config> config_;

  std::shared_ptr<RadioTask> radioTask_;
  std::shared_ptr<AudioTask> audioTask_;

  std::shared_ptr<PmService> pmService_;
  std::shared_ptr<HwMonitor> hwMonitor_;
  
  std::shared_ptr<Adafruit_SSD1306> display_;
  static std::shared_ptr<AiEsp32RotaryEncoder> rotaryEncoder_;

  // other
  volatile bool btnPressed_;

}; // Service

} // LoraDv

#endif // LORADV_SERVICE_H