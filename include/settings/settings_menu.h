#ifndef SETTINGS_MENU_H
#define SETTINGS_MENU_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <codec2.h>
#include <memory>
#include <sstream>
#include <iostream>

#include "settings/loradv_config.h"
#include "hal/radio_task.h"
#include "utils/utils.h"

namespace LoraDv {

class SettingsMenuItem {
public:
  SettingsMenuItem(std::shared_ptr<Config> config, int index) 
  : index_(index)
  , config_(config) {}
  virtual void changeValue(int delta) = 0;
  virtual void getName(std::stringstream &s) const = 0;
  virtual void getValue(std::stringstream &s) const = 0;
  virtual void select() {} 

protected:
  int index_;
  std::shared_ptr<Config> config_;
};

class SettingsMenu {
public:
  SettingsMenu(std::shared_ptr<Config> config);

  void draw(std::shared_ptr<Adafruit_SSD1306> display);

  void onEncoderPositionChanged(int delta);
  void onEncoderButtonClicked();

private:
  bool isValueSelected_;
  int selectedMenuItemIndex_;
  std::shared_ptr<Config> config_;
  std::vector<std::shared_ptr<SettingsMenuItem>> items_; // Updated type
};

} // LoraDv

#endif // SETTINGS_MENU_H