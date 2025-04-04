#include "settings/settings_menu.h"

namespace LoraDv {

SettingsMenu::SettingsMenu(shared_ptr<Config> config)
  : config_(config)
  , selectedMenuItemIndex_(0)
  , isValueSelected_(false)
{
  int i = 0;
  // frequency
  items_.push_back(make_shared<SettingsLoraFreqStepItem>(config, ++i));
  items_.push_back(make_shared<SettingsLoraFreqRxItem>(config, ++i));
  items_.push_back(make_shared<SettingsLoraFreqTxItem>(config, ++i));
  items_.push_back(make_shared<SettingsLoraPowerItem>(config, ++i));
  // modulation, codec
  items_.push_back(make_shared<SettingsModType>(config, ++i));
  items_.push_back(make_shared<SettingsAudioCodec>(config, ++i));
  // codec2
  items_.push_back(make_shared<SettingsAudioCodec2ModeItem>(config, ++i));
  items_.push_back(make_shared<SettingsAudioMaxPktSizeItem>(config, ++i));
  // opus
  items_.push_back(make_shared<SettingsAudioOpusRate>(config, ++i));
  items_.push_back(make_shared<SettingsAudioOpusPcmLen>(config, ++i));
  // audio
  items_.push_back(make_shared<SettingsAudioVolItem>(config, ++i));
  items_.push_back(make_shared<SettingsAudioEnablePrivacy>(config, ++i));
  // lora
  items_.push_back(make_shared<SettingsLoraBwItem>(config, ++i));
  items_.push_back(make_shared<SettingsLoraSfItem>(config, ++i));
  items_.push_back(make_shared<SettingsLoraCrItem>(config, ++i));
  // fsk
  items_.push_back(make_shared<SettingsFskBitRate>(config, ++i));
  items_.push_back(make_shared<SettingsFskFreqDev>(config, ++i));
  items_.push_back(make_shared<SettingsFskRxBw>(config, ++i));
  items_.push_back(make_shared<SettingsFskShaping>(config, ++i));
  // other
  items_.push_back(make_shared<SettingsBatteryMonCalItem>(config, ++i));
  items_.push_back(make_shared<SettingsPmLightSleepAfterMsItem>(config, ++i));
  items_.push_back(make_shared<SettingsSaveItem>(config, ++i));
  items_.push_back(make_shared<SettingsResetItem>(config, ++i));
  items_.push_back(make_shared<SettingsRebootItem>(config, ++i));
  items_.push_back(make_shared<SettingsInfoItem>(config, ++i));
}

void SettingsMenu::draw(shared_ptr<Adafruit_SSD1306> display) 
{
  stringstream s;
  items_[selectedMenuItemIndex_]->getName(s);
  s << endl;
  if (isValueSelected_) s << ">";
  items_[selectedMenuItemIndex_]->getValue(s);

  display->clearDisplay();
  display->setTextSize(1);
  display->setTextColor(WHITE);
  display->setCursor(0, 0);
  display->print(s.str().c_str());
  display->display();
}

void SettingsMenu::onEncoderPositionChanged(int delta)
{
  if (isValueSelected_) {
    items_[selectedMenuItemIndex_]->changeValue(delta);
  } else {
    int newIndex = selectedMenuItemIndex_ + delta;
    if (newIndex >= 0 && newIndex < items_.size()) selectedMenuItemIndex_ = newIndex;
  }
}

void SettingsMenu::onEncoderButtonClicked()
{
  isValueSelected_ = !isValueSelected_;
  if (isValueSelected_) {
    items_[selectedMenuItemIndex_]->select();
  }
}

} // LoraDv