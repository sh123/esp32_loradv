#include "settings/settings_menu.h"

namespace LoraDv {

SettingsMenu::SettingsMenu(shared_ptr<Config> config)
  : config_(config)
  , selectedMenuItemIndex_(0)
  , isValueSelected_(false)
{
  int i = 0;
  // frequency
  items_.push_back(shared_ptr<SettingsMenuItem>(new SettingsLoraFreqStepItem(config, ++i)));
  items_.push_back(shared_ptr<SettingsMenuItem>(new SettingsLoraFreqRxItem(config, ++i)));
  items_.push_back(shared_ptr<SettingsMenuItem>(new SettingsLoraFreqTxItem(config, ++i)));
  items_.push_back(shared_ptr<SettingsMenuItem>(new SettingsLoraPowerItem(config, ++i)));
  // modulation, codec
  items_.push_back(shared_ptr<SettingsMenuItem>(new SettingsModType(config, ++i)));
  items_.push_back(shared_ptr<SettingsMenuItem>(new SettingsAudioCodec(config, ++i)));
  // codec2
  items_.push_back(shared_ptr<SettingsMenuItem>(new SettingsAudioCodec2ModeItem(config, ++i)));
  items_.push_back(shared_ptr<SettingsMenuItem>(new SettingsAudioMaxPktSizeItem(config, ++i)));
  // opus
  items_.push_back(shared_ptr<SettingsMenuItem>(new SettingsAudioOpusRate(config, ++i)));
  items_.push_back(shared_ptr<SettingsMenuItem>(new SettingsAudioOpusPcmLen(config, ++i)));
  // audio
  items_.push_back(shared_ptr<SettingsMenuItem>(new SettingsAudioVolItem(config, ++i)));
  items_.push_back(shared_ptr<SettingsMenuItem>(new SettingsAudioEnablePrivacy(config, ++i)));
  // lora
  items_.push_back(shared_ptr<SettingsMenuItem>(new SettingsLoraBwItem(config, ++i)));
  items_.push_back(shared_ptr<SettingsMenuItem>(new SettingsLoraSfItem(config, ++i)));
  items_.push_back(shared_ptr<SettingsMenuItem>(new SettingsLoraCrItem(config, ++i)));
  // fsk
  items_.push_back(shared_ptr<SettingsMenuItem>(new SettingsFskBitRate(config, ++i)));
  items_.push_back(shared_ptr<SettingsMenuItem>(new SettingsFskFreqDev(config, ++i)));
  items_.push_back(shared_ptr<SettingsMenuItem>(new SettingsFskRxBw(config, ++i)));
  items_.push_back(shared_ptr<SettingsMenuItem>(new SettingsFskShaping(config, ++i)));
  // other
  items_.push_back(shared_ptr<SettingsMenuItem>(new SettingsBatteryMonCalItem(config, ++i)));
  items_.push_back(shared_ptr<SettingsMenuItem>(new SettingsPmLightSleepAfterMsItem(config, ++i)));
  items_.push_back(shared_ptr<SettingsMenuItem>(new SettingsSaveItem(config, ++i)));
  items_.push_back(shared_ptr<SettingsMenuItem>(new SettingsResetItem(config, ++i)));
  items_.push_back(shared_ptr<SettingsMenuItem>(new SettingsRebootItem(config, ++i)));
  items_.push_back(shared_ptr<SettingsMenuItem>(new SettingsInfoItem(config, ++i)));
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