#ifndef HW_MONITOR_H
#define HW_MONITOR_H

#include <Arduino.h>
#include <memory>
#include "loradv_config.h"

namespace LoraDv {

class HwMonitor {

public:
  HwMonitor();
  void setup(std::shared_ptr<const Config> config);

  float getBatteryVoltage() const;

private:
  std::shared_ptr<const Config> config_;

};

} // LoraDv

#endif // HW_MONITOR_H