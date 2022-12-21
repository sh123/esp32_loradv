#ifndef HW_MONITOR_H
#define HW_MONITOR_H

#include <Arduino.h>

#include "loradv_config.h"

namespace LoraDv {

class HwMonitor {

public:
  HwMonitor();
  void setup(const Config &config);

  float getBatteryVoltage() const;

private:
  Config config_;

};

} // LoraDv

#endif // HW_MONITOR_H