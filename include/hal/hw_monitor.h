#ifndef HW_MONITOR_H
#define HW_MONITOR_H

#include <Arduino.h>
#include <memory>
#include "settings/config.h"

using namespace std;

namespace LoraDv {

class HwMonitor {

public:
  HwMonitor(shared_ptr<const Config> config);
  
  float getBatteryVoltage() const;

private:
  shared_ptr<const Config> config_;

};

} // LoraDv

#endif // HW_MONITOR_H