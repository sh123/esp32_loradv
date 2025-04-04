#ifndef LORADV_SERVICE_H
#define LORADV_SERVICE_H

#include <Arduino.h>
#include <memory>
#define DEBUGLOG_DEFAULT_LOG_LEVEL_INFO
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

#include "settings/loradv_config.h"
#include "hal/radio_task.h"
#include "audio/audio_task.h"
#include "hal/pm_service.h"
#include "hal/hw_monitor.h"
#include "settings/settings_menu.h"

namespace LoraDv {

class Service {

public:
  Service(std::shared_ptr<Config> conf);

  void setup();
  void loop();

private:
  constexpr static int CfgDisplayWidth = 128;                // display width
  constexpr static int CfgDisplayHeight = 32;                // display height

  constexpr static int CfgEncoderBtnLongMs = 2000;           // encoder long button press

private:
  void setupEncoder();
  void setupScreen();

  static IRAM_ATTR void isrReadEncoder();

  void updateScreen() const;

  bool processPttButton();
  bool processRotaryEncoder();

private:
  std::shared_ptr<Config> config_;

  std::shared_ptr<Adafruit_SSD1306> display_;
  static std::shared_ptr<AiEsp32RotaryEncoder> rotaryEncoder_;

  std::shared_ptr<PmService> pmService_;
  std::shared_ptr<HwMonitor> hwMonitor_;

  std::shared_ptr<RadioTask> radioTask_;
  std::shared_ptr<AudioTask> audioTask_;

  std::shared_ptr<SettingsMenu> settingsMenu_;

  // other
  volatile bool btnPressed_;

}; // Service

} // LoraDv

#endif // LORADV_SERVICE_H