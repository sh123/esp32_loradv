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
#include "settings/settings_menu_item.h"
#include "hal/radio_task.h"
#include "utils/utils.h"

namespace LoraDv {

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