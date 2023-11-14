#include "settings_menu.h"

namespace LoraDv {

class SettingsLoraFreqStepItem : public SettingsItem {
private:
  static const int CfgItemsCount = 7;
public:
  SettingsLoraFreqStepItem(std::shared_ptr<Config> config) 
    : SettingsItem(config)
    , items_{ 1000, 5000, 6250, 10000, 12500, 20000, 25000 } 
  {
    for (selIndex_ = 0; selIndex_ < CfgItemsCount; selIndex_++)
      if (config_->LoraFreqStep == items_[selIndex_])
        break;
  }
  void changeValue(int delta) {
    int newIndex = selIndex_ + delta;
    if (newIndex >= 0 && newIndex < CfgItemsCount) selIndex_ = newIndex;
    config_->LoraFreqStep = items_[selIndex_];
  }
  void getName(std::stringstream &s) const { s << "1.Freq Step"; }
  void getValue(std::stringstream &s) const { s << config_->LoraFreqStep << "Hz"; }
private:
  int selIndex_;
  long items_[CfgItemsCount];
};

class SettingsLoraFreqRxItem : public SettingsItem {
public:
  SettingsLoraFreqRxItem(std::shared_ptr<Config> config) : SettingsItem(config) {}
  void changeValue(int delta) { 
    long newVal = config_->LoraFreqRx + config_->LoraFreqStep * delta;
    if (newVal >= 400e6 && newVal <= 520e6) config_->LoraFreqRx = newVal;
  }
  void getName(std::stringstream &s) const { s << "2.RX Freq"; }
  void getValue(std::stringstream &s) const { s << config_->LoraFreqRx << "Hz"; }
};

class SettingsLoraFreqTxItem : public SettingsItem {
public:
  SettingsLoraFreqTxItem(std::shared_ptr<Config> config) : SettingsItem(config) {}
  void changeValue(int delta) {
    long newVal = config_->LoraFreqTx + config_->LoraFreqStep * delta;
    if (newVal >= 400e6 && newVal <= 520e6) config_->LoraFreqTx = newVal;
  }
  void getName(std::stringstream &s) const { s << "3.TX Freq"; }
  void getValue(std::stringstream &s) const { s << config_->LoraFreqTx << "Hz"; }
};

class SettingsLoraBwItem : public SettingsItem {
private:
  static const int CfgItemsCount = 10;
public:
  SettingsLoraBwItem(std::shared_ptr<Config> config) 
    : SettingsItem(config)
    , items_{ 7800, 10400, 15600, 20800, 31250, 41700, 62500, 125000, 250000, 500000 } 
  {
    for (selIndex_ = 0; selIndex_ < CfgItemsCount; selIndex_++)
      if (config_->LoraBw == items_[selIndex_])
        break;
  }
  void changeValue(int delta) {
    int newIndex = selIndex_ + delta;
    if (newIndex >= 0 && newIndex < CfgItemsCount) selIndex_ = newIndex;
    config_->LoraBw = items_[selIndex_];
  }
  void getName(std::stringstream &s) const { s << "4.Bandwidth"; }
  void getValue(std::stringstream &s) const { s << config_->LoraBw << "Hz"; }
private:
  int selIndex_;
  long items_[CfgItemsCount];
};

class SettingsLoraSfItem : public SettingsItem {
public:
  SettingsLoraSfItem(std::shared_ptr<Config> config) : SettingsItem(config) {}
  void changeValue(int delta) { 
    long newVal = config_->LoraSf + delta;
    if (newVal >= 7 && newVal <= 12) config_->LoraSf = newVal;
  }
  void getName(std::stringstream &s) const { s << "5.Spreading"; }
  void getValue(std::stringstream &s) const { s << config_->LoraSf; }
};

class SettingsLoraCrItem : public SettingsItem {
public:
  SettingsLoraCrItem(std::shared_ptr<Config> config) : SettingsItem(config) {}
  void changeValue(int delta) { 
    long newVal = config_->LoraCodingRate + delta;
    if (newVal >= 5 && newVal <= 8) config_->LoraCodingRate = newVal;
  }
  void getName(std::stringstream &s) const { s << "6.Coding Rate"; }
  void getValue(std::stringstream &s) const { s << config_->LoraCodingRate; }
};

class SettingsLoraPowerItem : public SettingsItem {
public:
  SettingsLoraPowerItem(std::shared_ptr<Config> config) : SettingsItem(config) {}
  void changeValue(int delta) { 
    long newVal = config_->LoraPower + delta;
    if (newVal >= 2 && newVal <= 22) config_->LoraPower = newVal;
  }
  void getName(std::stringstream &s) const { s << "7.Power"; }
  void getValue(std::stringstream &s) const { s << config_->LoraPower << "dBm"; }
};

class SettingsAudioCodec2ModeItem : public SettingsItem {
private:
  static const int CfgItemsCount = 8;
public:
  SettingsAudioCodec2ModeItem(std::shared_ptr<Config> config) 
    : SettingsItem(config)
    , items_{ 
      CODEC2_MODE_700C,
      CODEC2_MODE_1200,
      CODEC2_MODE_1300,
      CODEC2_MODE_1400,
      CODEC2_MODE_1600,
      CODEC2_MODE_2400,
      CODEC2_MODE_3200
    }
    , map_{ 
      { CODEC2_MODE_700C, "700" },
      { CODEC2_MODE_1200, "1200" },
      { CODEC2_MODE_1300, "1300" },
      { CODEC2_MODE_1400, "1400" },
      { CODEC2_MODE_1600, "1600" },
      { CODEC2_MODE_2400, "2400" },
      { CODEC2_MODE_3200, "3200" }
    }
  {
    for (selIndex_ = 0; selIndex_ < CfgItemsCount; selIndex_++)
      if (config_->AudioCodec2Mode == items_[selIndex_])
        break;
  }
  void changeValue(int delta) {
    int newIndex = selIndex_ + delta;
    if (newIndex >= 0 && newIndex < CfgItemsCount) selIndex_ = newIndex;
    config_->AudioCodec2Mode = items_[selIndex_];
  }
  void getName(std::stringstream &s) const { s << "8.Codec2 Mode"; }
  void getValue(std::stringstream &s) const { 
    for (int i = 0; i < CfgItemsCount; i++)
      if (config_->AudioCodec2Mode == map_[i].k) {
        s << map_[i].val << "bps"; 
        break;
      }
  }
private:
  int selIndex_;
  long items_[CfgItemsCount];
  struct MapItem { 
    int k; 
    const char *val; 
  } map_[CfgItemsCount];
};

class SettingsAudioVolItem : public SettingsItem {
public:
  SettingsAudioVolItem(std::shared_ptr<Config> config) : SettingsItem(config) {}
  void changeValue(int delta) { 
    long newVal = config_->AudioVol + delta;
    if (newVal >= 0 && newVal <= CFG_AUDIO_MAX_VOL) config_->AudioVol = newVal;
  }
  void getName(std::stringstream &s) const { s << "9.Volume"; }
  void getValue(std::stringstream &s) const { s << config_->AudioVol << "%"; }
};

class SettingsBatteryMonCalItem : public SettingsItem {
public:
  SettingsBatteryMonCalItem(std::shared_ptr<Config> config) : SettingsItem(config) {}
  void changeValue(int delta) { 
    float newVal = config_->BatteryMonCal + 0.01f * delta;
    if (newVal >= -2.0f && newVal <= 2.0f) config_->BatteryMonCal = newVal;
  }
  void getName(std::stringstream &s) const { s << "10.Battery Cal"; }
  void getValue(std::stringstream &s) const { s << config_->BatteryMonCal << "V"; }
};

class SettingsPmLightSleepAfterMsItem : public SettingsItem {
public:
  SettingsPmLightSleepAfterMsItem(std::shared_ptr<Config> config) : SettingsItem(config) {}
  void changeValue(int delta) { 
    int newVal = config_->PmSleepAfterMs + 1000 * delta;
    if (newVal >= 10*1000 && newVal <= 5*60*1000) config_->PmSleepAfterMs = newVal;
  }
  void getName(std::stringstream &s) const { s << "11.Sleep"; }
  void getValue(std::stringstream &s) const { s << config_->PmSleepAfterMs / 1000 << "s"; }
};

class SettingsSaveItem : public SettingsItem {
public:
  SettingsSaveItem(std::shared_ptr<Config> config) : SettingsItem(config) {}
  void changeValue(int delta) { }
  void getName(std::stringstream &s) const { s << "12.Save Settings"; }
  void getValue(std::stringstream &s) const { s << "Click to save"; }
  void select() { config_->Save(); }
};

class SettingsResetItem : public SettingsItem {
public:
  SettingsResetItem(std::shared_ptr<Config> config) : SettingsItem(config) {}
  void changeValue(int delta) { }
  void getName(std::stringstream &s) const { s << "13.Reset Settings"; }
  void getValue(std::stringstream &s) const { s << "Click to reset"; }
  void select() { config_->Reset(); }
};

class SettingsRebootItem : public SettingsItem {
public:
  SettingsRebootItem(std::shared_ptr<Config> config) : SettingsItem(config) {}
  void changeValue(int delta) { }
  void getName(std::stringstream &s) const { s << "14.Reboot"; }
  void getValue(std::stringstream &s) const { s << "Click to reboot"; }
  void select() { ESP.restart(); }
};

class SettingsInfoItem : public SettingsItem {
public:
  SettingsInfoItem(std::shared_ptr<Config> config) : SettingsItem(config) {}
  void changeValue(int delta) { }
  void getName(std::stringstream &s) const { s << "15.Info"; }
  void getValue(std::stringstream &s) const { 
    s << "App:" << LORADV_VERSION << " Conf:" << config_->Version << std::endl;
    s << "Free:" << esp_get_free_heap_size() << "KB" << std::endl;
    s << "RF:" << RadioTask::getSpeed(config_->LoraSf, config_->LoraCodingRate, config_->LoraBw) << "bps/";
    s << RadioTask::getSnrLimit(config_->LoraSf, config_->LoraBw);
  }
};

SettingsMenu::SettingsMenu(std::shared_ptr<Config> config)
  : config_(config)
  , selectedMenuItemIndex_(0)
  , isValueSelected_(false)
{
  items_.push_back(std::shared_ptr<SettingsItem>(new SettingsLoraFreqStepItem(config)));
  items_.push_back(std::shared_ptr<SettingsItem>(new SettingsLoraFreqRxItem(config)));
  items_.push_back(std::shared_ptr<SettingsItem>(new SettingsLoraFreqTxItem(config)));
  items_.push_back(std::shared_ptr<SettingsItem>(new SettingsLoraBwItem(config)));
  items_.push_back(std::shared_ptr<SettingsItem>(new SettingsLoraSfItem(config)));
  items_.push_back(std::shared_ptr<SettingsItem>(new SettingsLoraCrItem(config)));
  items_.push_back(std::shared_ptr<SettingsItem>(new SettingsLoraPowerItem(config)));
  items_.push_back(std::shared_ptr<SettingsItem>(new SettingsAudioCodec2ModeItem(config)));
  items_.push_back(std::shared_ptr<SettingsItem>(new SettingsAudioVolItem(config)));
  items_.push_back(std::shared_ptr<SettingsItem>(new SettingsBatteryMonCalItem(config)));
  items_.push_back(std::shared_ptr<SettingsItem>(new SettingsPmLightSleepAfterMsItem(config)));
  items_.push_back(std::shared_ptr<SettingsItem>(new SettingsSaveItem(config)));
  items_.push_back(std::shared_ptr<SettingsItem>(new SettingsResetItem(config)));
  items_.push_back(std::shared_ptr<SettingsItem>(new SettingsRebootItem(config)));
  items_.push_back(std::shared_ptr<SettingsItem>(new SettingsInfoItem(config)));
}

void SettingsMenu::draw(std::shared_ptr<Adafruit_SSD1306> display) 
{
  std::stringstream s;
  items_[selectedMenuItemIndex_]->getName(s);
  s << std::endl;
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