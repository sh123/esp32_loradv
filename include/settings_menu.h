#ifndef SETTINGS_MENU_H
#define SETTINGS_MENU_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <codec2.h>
#include <memory>
#include <sstream>

#include "loradv_config.h"

namespace LoraDv {

class SettingsItem {
public:
  SettingsItem(std::shared_ptr<Config> config) : config_(config) {}
  virtual void changeValue(int delta) = 0;
  virtual void getName(std::stringstream &s) const = 0;
  virtual void getValue(std::stringstream &s) const = 0;

protected:
  std::shared_ptr<Config> config_;
};

class SettingsMenu {
private:
  static const int CfgItemsCount = 10;

public:
  SettingsMenu();
};

} // LoraDv

#endif // SETTINGS_MENU_H