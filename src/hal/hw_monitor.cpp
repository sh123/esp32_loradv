#include "hal/hw_monitor.h"

namespace LoraDv {

HwMonitor::HwMonitor(std::shared_ptr<const Config> config) 
  : config_(config)
{
}

float HwMonitor::getBatteryVoltage() const
{
  int bat_value = analogRead(config_->BatteryMonPin_);
  return 2 * bat_value * (3.3 / 4096.0) + config_->BatteryMonCal;
}

} // LoraDv