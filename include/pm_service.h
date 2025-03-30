#ifndef PM_SERVICE_H
#define PM_SERVICE_H

#include <Arduino.h>
#include <memory>
#include <arduino-timer.h>
#include <Adafruit_SSD1306.h>

#include "settings/loradv_config.h"

namespace LoraDv {

class PmService {

public:
  PmService();

  void setup(std::shared_ptr<const Config> config, std::shared_ptr<Adafruit_SSD1306> display);
  bool loop();

  void lightSleepReset();

private:
  static bool lightSleepEnterTimer(void *param);
  void lightSleepEnter();
  esp_sleep_wakeup_cause_t lightSleepWait(uint64_t sleepTimeUs) const;

private:
  std::shared_ptr<const Config> config_;
  std::shared_ptr<Adafruit_SSD1306> display_;

  Timer<1> lightSleepTimer_;
  Timer<1>::Task lightSleepTimerTask_;

  bool isExitFromSleep_;
};

} // LoraDv

#endif // PM_SERVICE_H