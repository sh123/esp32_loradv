#ifndef SETTINGS_MENU_ITEM_H
#define SETTINGS_MENU_ITEM_H

#include <Arduino.h>
#include <memory>
#include <sstream>
#include <iostream>

#include "settings/config.h"
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

class SettingsLoraFreqStepItem : public SettingsMenuItem {
private:
  static const int CfgItemsCount = 7;
public:
  SettingsLoraFreqStepItem(std::shared_ptr<Config> config, int index) 
    : SettingsMenuItem(config, index)
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
  void getName(std::stringstream &s) const { s << index_ << ".Freq Step"; }
  void getValue(std::stringstream &s) const { s << config_->LoraFreqStep << "Hz"; }
private:
  int selIndex_;
  long items_[CfgItemsCount];
};

class SettingsLoraFreqRxItem : public SettingsMenuItem {
public:
  SettingsLoraFreqRxItem(std::shared_ptr<Config> config, int index) : SettingsMenuItem(config, index) {}
  void changeValue(int delta) { 
    long newVal = config_->LoraFreqRx + config_->LoraFreqStep * delta;
    if (newVal >= 400e6 && newVal <= 520e6) config_->LoraFreqRx = newVal;
  }
  void getName(std::stringstream &s) const { s << index_ << ".RX Freq"; }
  void getValue(std::stringstream &s) const { s << config_->LoraFreqRx << "Hz"; }
};

class SettingsLoraFreqTxItem : public SettingsMenuItem {
public:
  SettingsLoraFreqTxItem(std::shared_ptr<Config> config, int index) : SettingsMenuItem(config, index) {}
  void changeValue(int delta) {
    long newVal = config_->LoraFreqTx + config_->LoraFreqStep * delta;
    if (newVal >= 400e6 && newVal <= 520e6) config_->LoraFreqTx = newVal;
  }
  void getName(std::stringstream &s) const { s << index_ << ".TX Freq"; }
  void getValue(std::stringstream &s) const { s << config_->LoraFreqTx << "Hz"; }
};

class SettingsLoraPowerItem : public SettingsMenuItem {
public:
  SettingsLoraPowerItem(std::shared_ptr<Config> config, int index) : SettingsMenuItem(config, index) {}
  void changeValue(int delta) { 
    int newVal = config_->LoraPower + delta;
    if (newVal >= -9 && newVal <= 22) config_->LoraPower = newVal;
  }
  void getName(std::stringstream &s) const { s << index_ << ".Power"; }
  void getValue(std::stringstream &s) const { s << config_->LoraPower << "dBm"; }
};

class SettingsModType : public SettingsMenuItem {
private:
  static const int CfgItemsCount = 2;
public:
  SettingsModType(std::shared_ptr<Config> config, int index)
    : SettingsMenuItem(config, index)
    , items_{ 
      CFG_AUDIO_CODEC_CODEC2,
      CFG_AUDIO_CODEC_OPUS
    }
    , map_{ 
      { CFG_AUDIO_CODEC_CODEC2, "Codec2" },
      { CFG_AUDIO_CODEC_OPUS, "OPUS" }
    }
  {
    for (selIndex_ = 0; selIndex_ < CfgItemsCount; selIndex_++)
      if (config_->AudioCodec == items_[selIndex_])
        break;
  }
  void changeValue(int delta) {
    int newIndex = selIndex_ + delta;
    if (newIndex >= 0 && newIndex < CfgItemsCount) selIndex_ = newIndex;
    config_->AudioCodec = items_[selIndex_];
  }
  void getName(std::stringstream &s) const { s << index_ << ".Audio Codec"; }
  void getValue(std::stringstream &s) const { 
    for (int i = 0; i < CfgItemsCount; i++)
      if (config_->AudioCodec == map_[i].k) {
        s << map_[i].val; 
        break;
      }
  }
private:
  int selIndex_;
  int items_[CfgItemsCount];
  struct MapItem { 
    int k; 
    const char *val; 
  } map_[CfgItemsCount];
};

class SettingsAudioCodec2ModeItem : public SettingsMenuItem {
private:
  static const int CfgItemsCount = 8;
public:
  SettingsAudioCodec2ModeItem(std::shared_ptr<Config> config, int index) 
    : SettingsMenuItem(config, index)
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
  void getName(std::stringstream &s) const { s << index_ << ".Codec2 Mode"; }
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

class SettingsAudioMaxPktSizeItem : public SettingsMenuItem {
public:
  SettingsAudioMaxPktSizeItem(std::shared_ptr<Config> config, int index) : SettingsMenuItem(config, index) {}
  void changeValue(int delta) { 
    long newVal = config_->AudioMaxPktSize + delta;
    if (newVal >= 8 && newVal <= 240) config_->AudioMaxPktSize = newVal;
  }
  void getName(std::stringstream &s) const { s << index_ << ".Codec2 pkt size"; }
  void getValue(std::stringstream &s) const { s << config_->AudioMaxPktSize << "bytes"; }
};

class SettingsAudioOpusRate : public SettingsMenuItem {
public:
  SettingsAudioOpusRate(std::shared_ptr<Config> config, int index) : SettingsMenuItem(config, index) {}
  void changeValue(int delta) { 
    int newVal = config_->AudioOpusRate + 100 * delta;
    if (newVal >= 2400 && newVal <= 512000) config_->AudioOpusRate = newVal;
  }
  void getName(std::stringstream &s) const { s << index_ << ".OPUS Rate"; }
  void getValue(std::stringstream &s) const { s << config_->AudioOpusRate << "bps"; }
};

class SettingsAudioOpusPcmLen : public SettingsMenuItem {
private:
  static const int CfgItemsCount = 9;
public:
  SettingsAudioOpusPcmLen(std::shared_ptr<Config> config, int index) 
    : SettingsMenuItem(config, index)
    , items_{ 2.5, 5, 10, 20, 40, 60, 80, 100, 120 }
  {
    for (selIndex_ = 0; selIndex_ < CfgItemsCount; selIndex_++)
      if (config_->AudioOpusPcmLen == items_[selIndex_])
        break;
  }
  void changeValue(int delta) {
    int newIndex = selIndex_ + delta;
    if (newIndex >= 0 && newIndex < CfgItemsCount) selIndex_ = newIndex;
    config_->AudioOpusPcmLen = items_[selIndex_];
  }
  void getName(std::stringstream &s) const { s << index_ << ".OPUS PCM Len"; }
  void getValue(std::stringstream &s) const { s << config_->AudioOpusPcmLen << "ms"; }
private:
  int selIndex_;
  float items_[CfgItemsCount];
};

class SettingsAudioVolItem : public SettingsMenuItem {
public:
  SettingsAudioVolItem(std::shared_ptr<Config> config, int index) : SettingsMenuItem(config, index) {}
  void changeValue(int delta) { 
    long newVal = config_->AudioVol + delta;
    if (newVal >= 0 && newVal <= CFG_AUDIO_MAX_VOL) config_->AudioVol = newVal;
  }
  void getName(std::stringstream &s) const { s << index_ << ".Volume"; }
  void getValue(std::stringstream &s) const { s << config_->AudioVol; }
};

class SettingsAudioEnablePrivacy : public SettingsMenuItem {
public:
  SettingsAudioEnablePrivacy(std::shared_ptr<Config> config, int index) : SettingsMenuItem(config, index) {}
  void changeValue(int delta) { 
    config_->AudioEnPriv = !config_->AudioEnPriv;
  }
  void getName(std::stringstream &s) const { s << index_ << ".Privacy"; }
  void getValue(std::stringstream &s) const { s << (config_->AudioEnPriv ? "ON" : "OFF"); }
};

class SettingsAudioCodec : public SettingsMenuItem {
private:
  static const int CfgItemsCount = 2;
public:
  SettingsAudioCodec(std::shared_ptr<Config> config, int index)
    : SettingsMenuItem(config, index)
    , items_{ 
      CFG_MOD_TYPE_LORA,
      CFG_MOD_TYPE_FSK
    }
    , map_{ 
      { CFG_MOD_TYPE_LORA, "LoRa" },
      { CFG_MOD_TYPE_FSK, "FSK" }
    }
  {
    for (selIndex_ = 0; selIndex_ < CfgItemsCount; selIndex_++)
      if (config_->ModType == items_[selIndex_])
        break;
  }
  void changeValue(int delta) {
    int newIndex = selIndex_ + delta;
    if (newIndex >= 0 && newIndex < CfgItemsCount) selIndex_ = newIndex;
    config_->ModType = items_[selIndex_];
  }
  void getName(std::stringstream &s) const { s << index_ << ".Modulation"; }
  void getValue(std::stringstream &s) const { 
    for (int i = 0; i < CfgItemsCount; i++)
      if (config_->ModType == map_[i].k) {
        s << map_[i].val; 
        break;
      }
  }
private:
  int selIndex_;
  int items_[CfgItemsCount];
  struct MapItem { 
    int k; 
    const char *val; 
  } map_[CfgItemsCount];
};

class SettingsLoraBwItem : public SettingsMenuItem {
private:
  static const int CfgItemsCount = 10;
public:
  SettingsLoraBwItem(std::shared_ptr<Config> config, int index) 
    : SettingsMenuItem(config, index)
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
  void getName(std::stringstream &s) const { s << index_ << ".LoRa Bandwidth"; }
  void getValue(std::stringstream &s) const { s << config_->LoraBw << "Hz"; }
private:
  int selIndex_;
  long items_[CfgItemsCount];
};

class SettingsLoraSfItem : public SettingsMenuItem {
public:
  SettingsLoraSfItem(std::shared_ptr<Config> config, int index) : SettingsMenuItem(config, index) {}
  void changeValue(int delta) { 
    long newVal = config_->LoraSf + delta;
    if (newVal >= 7 && newVal <= 12) config_->LoraSf = newVal;
  }
  void getName(std::stringstream &s) const { s << index_ << ".LoRa Spreading"; }
  void getValue(std::stringstream &s) const { s << config_->LoraSf; }
};

class SettingsLoraCrItem : public SettingsMenuItem {
public:
  SettingsLoraCrItem(std::shared_ptr<Config> config, int index) : SettingsMenuItem(config, index) {}
  void changeValue(int delta) { 
    long newVal = config_->LoraCodingRate + delta;
    if (newVal >= 5 && newVal <= 8) config_->LoraCodingRate = newVal;
  }
  void getName(std::stringstream &s) const { s << index_ << ".LoRa Coding Rate"; }
  void getValue(std::stringstream &s) const { s << config_->LoraCodingRate; }
};

class SettingsFskBitRate : public SettingsMenuItem {
public:
  SettingsFskBitRate(std::shared_ptr<Config> config, int index) : SettingsMenuItem(config, index) {}
  void changeValue(int delta) { 
    float newVal = config_->FskBitRate + (float)delta/10.0;
    if (newVal >= 0.6 && newVal <= 300.0) config_->FskBitRate = newVal;
  }
  void getName(std::stringstream &s) const { s << index_ << ".FSK Bit Rate"; }
  void getValue(std::stringstream &s) const { s << config_->FskBitRate << "kbps"; }
};

class SettingsFskFreqDev : public SettingsMenuItem {
public:
  SettingsFskFreqDev(std::shared_ptr<Config> config, int index) : SettingsMenuItem(config, index) {}
  void changeValue(int delta) { 
    float newVal = config_->FskFreqDev + (float)delta/10.0;
    if (newVal >= 0.6 && newVal <= 200.0) config_->FskFreqDev = newVal;
  }
  void getName(std::stringstream &s) const { s << index_ << ".FSK Freq Dev"; }
  void getValue(std::stringstream &s) const { s << config_->FskFreqDev << "kHz"; }
};

class SettingsFskRxBw : public SettingsMenuItem {
private:
  static const int CfgItemsCount = 21;
public:
  SettingsFskRxBw(std::shared_ptr<Config> config, int index) 
    : SettingsMenuItem(config, index)
    , items_{ 4.8, 5.8, 7.3, 9.7, 11.7, 14.6, 19.5, 23.4, 29.3, 39.0, 46.9, 58.6, 
              78.2, 93.8, 117.3, 156.2, 187.2, 234.3, 312.0, 373.0, 467.0 } 
  {
    for (selIndex_ = 0; selIndex_ < CfgItemsCount; selIndex_++)
      if (config_->FskRxBw == items_[selIndex_])
        break;
  }
  void changeValue(int delta) {
    int newIndex = selIndex_ + delta;
    if (newIndex >= 0 && newIndex < CfgItemsCount) selIndex_ = newIndex;
    config_->FskRxBw = items_[selIndex_];
  }
  void getName(std::stringstream &s) const { s << index_ << ".FSK RX BW"; }
  void getValue(std::stringstream &s) const { s << config_->FskRxBw << "kHz"; }
private:
  int selIndex_;
  float items_[CfgItemsCount];
};

class SettingsFskShaping : public SettingsMenuItem {
private:
  static const int CfgItemsCount = 5;
public:
  SettingsFskShaping(std::shared_ptr<Config> config, int index)
    : SettingsMenuItem(config, index)
    , items_{ 
      RADIOLIB_SHAPING_NONE,
      RADIOLIB_SHAPING_0_3,
      RADIOLIB_SHAPING_0_5,
      RADIOLIB_SHAPING_0_7,
      RADIOLIB_SHAPING_1_0
    }
    , map_{ 
      { RADIOLIB_SHAPING_NONE, "None" },
      { RADIOLIB_SHAPING_0_3, "0.3" },
      { RADIOLIB_SHAPING_0_5, "0.5" },
      { RADIOLIB_SHAPING_0_7, "0.7" },
      { RADIOLIB_SHAPING_1_0, "1.0" },
    }
  {
    for (selIndex_ = 0; selIndex_ < CfgItemsCount; selIndex_++)
      if (config_->FskShaping == items_[selIndex_])
        break;
  }
  void changeValue(int delta) {
    int newIndex = selIndex_ + delta;
    if (newIndex >= 0 && newIndex < CfgItemsCount) selIndex_ = newIndex;
    config_->FskShaping = items_[selIndex_];
  }
  void getName(std::stringstream &s) const { s << index_ << ".FSK Shaping"; }
  void getValue(std::stringstream &s) const { 
    for (int i = 0; i < CfgItemsCount; i++)
      if (config_->FskShaping == map_[i].k) {
        s << map_[i].val; 
        break;
      }
  }
private:
  int selIndex_;
  int items_[CfgItemsCount];
  struct MapItem { 
    int k; 
    const char *val; 
  } map_[CfgItemsCount];
};

class SettingsBatteryMonCalItem : public SettingsMenuItem {
public:
  SettingsBatteryMonCalItem(std::shared_ptr<Config> config, int index) : SettingsMenuItem(config, index) {}
  void changeValue(int delta) { 
    float newVal = config_->BatteryMonCal + 0.01f * delta;
    if (newVal >= -2.0f && newVal <= 2.0f) config_->BatteryMonCal = newVal;
  }
  void getName(std::stringstream &s) const { s << index_ << ".Battery Cal"; }
  void getValue(std::stringstream &s) const { s << config_->BatteryMonCal << "V"; }
};

class SettingsPmLightSleepAfterMsItem : public SettingsMenuItem {
public:
  SettingsPmLightSleepAfterMsItem(std::shared_ptr<Config> config, int index) : SettingsMenuItem(config, index) {}
  void changeValue(int delta) { 
    int newVal = config_->PmSleepAfterMs + 1000 * delta;
    if (newVal >= 10*1000 && newVal <= 5*60*1000) config_->PmSleepAfterMs = newVal;
  }
  void getName(std::stringstream &s) const { s << index_ << ".Sleep"; }
  void getValue(std::stringstream &s) const { s << config_->PmSleepAfterMs / 1000 << "s"; }
};

class SettingsSaveItem : public SettingsMenuItem {
public:
  SettingsSaveItem(std::shared_ptr<Config> config, int index) : SettingsMenuItem(config, index) {}
  void changeValue(int delta) { }
  void getName(std::stringstream &s) const { s << index_ << ".Save Settings"; }
  void getValue(std::stringstream &s) const { s << "Click to save"; }
  void select() { config_->Save(); ESP.restart(); }
};

class SettingsResetItem : public SettingsMenuItem {
public:
  SettingsResetItem(std::shared_ptr<Config> config, int index) : SettingsMenuItem(config, index) {}
  void changeValue(int delta) { }
  void getName(std::stringstream &s) const { s << index_ << ".Reset Settings"; }
  void getValue(std::stringstream &s) const { s << "Click to reset"; }
  void select() { config_->Reset(); ESP.restart(); }
};

class SettingsRebootItem : public SettingsMenuItem {
public:
  SettingsRebootItem(std::shared_ptr<Config> config, int index) : SettingsMenuItem(config, index) {}
  void changeValue(int delta) { }
  void getName(std::stringstream &s) const { s << index_ << ".Reboot"; }
  void getValue(std::stringstream &s) const { s << "Click to reboot"; }
  void select() { ESP.restart(); }
};

class SettingsInfoItem : public SettingsMenuItem {
public:
  SettingsInfoItem(std::shared_ptr<Config> config, int index) : SettingsMenuItem(config, index) {}
  void changeValue(int delta) { }
  void getName(std::stringstream &s) const { s << index_ << ".Info"; }
  void getValue(std::stringstream &s) const { 
    s << "App:" << LORADV_VERSION << " Conf:" << config_->Version << std::endl;
    s << "Free:" << esp_get_free_heap_size() << "KB" << std::endl;
    s << "RF:" << Utils::loraGetSpeed(config_->LoraSf, config_->LoraCodingRate, config_->LoraBw) << "bps/";
    s << Utils::loraGetSnrLimit(config_->LoraSf, config_->LoraBw);
  }
};

} // LoraDv

#endif // SETTIGS_MENU_ITEM_H