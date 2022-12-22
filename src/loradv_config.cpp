#include "loradv_config.h"

namespace LoraDv {

#define N(v) #v

Config::Config()
{
}

void Config::Load()
{
  LOG_INFO("Loading settings");
  prefs_.begin("LoraDv");

  // uninitialized, save
  if (!prefs_.isKey(N(Version))) {
    LOG_INFO("Not loaded, first time settings");
    Save();
    prefs_.end();
    return;
  }
  // new default version is higher, save and use default settings
  int version = prefs_.getInt(N(Version));
  if (Version > version) {
    LOG_INFO("Not loaded, new default settings version");
    Save();
    prefs_.end();
    return;
  }
  if (prefs_.isKey(N(LoraFreqRx))) {
    LoraFreqRx = prefs_.getLong(N(LoraFreqRx));
  } else {
    prefs_.putLong(N(LoraFreqRx), LoraFreqRx);
  }
  if (prefs_.isKey(N(LoraFreqTx))) {
    LoraFreqTx = prefs_.getLong(N(LoraFreqTx));
  } else {
    prefs_.putLong(N(LoraFreqTx), LoraFreqTx);
  }
  if (prefs_.isKey(N(LoraBw))) {
    LoraBw = prefs_.getLong(N(LoraBw));
  } else {
    prefs_.putLong(N(LoraBw), LoraBw);
  }
  if (prefs_.isKey(N(LoraSf))) {
    LoraSf = prefs_.getInt(N(LoraSf));
  } else {
    prefs_.putInt(N(LoraSf), LoraSf);
  }
  if (prefs_.isKey(N(LoraCodingRate))) {
    LoraCodingRate = prefs_.getInt(N(LoraCodingRate));
  } else {
    prefs_.putInt(N(LoraCodingRate), LoraCodingRate);
  }
  if (prefs_.isKey(N(LoraPower))) {
    LoraPower = prefs_.getInt(N(LoraPower));
  } else {
    prefs_.putInt(N(LoraPower), LoraPower);
  }
  if (prefs_.isKey(N(AudioCodec2Mode))) {
    AudioCodec2Mode = prefs_.getInt(N(AudioCodec2Mode));
  } else {
    prefs_.putInt(N(AudioCodec2Mode), AudioCodec2Mode);
  }
  if (prefs_.isKey(N(AudioVol))) {
    AudioVol = prefs_.getInt(N(AudioVol));
  } else {
    prefs_.putInt(N(AudioVol), AudioVol);
  }
  if (prefs_.isKey(N(BatteryMonCal))) {
    BatteryMonCal = prefs_.getInt(N(BatteryMonCal));
  } else {
    prefs_.putInt(N(BatteryMonCal), BatteryMonCal);
  }
  if (prefs_.isKey(N(PmLightSleepAfterMs))) {
    PmLightSleepAfterMs = prefs_.getInt(N(PmLightSleepAfterMs));
  } else {
    prefs_.putInt(N(PmLightSleepAfterMs), PmLightSleepAfterMs);
  }
  prefs_.end();
  LOG_INFO("Settings are loaded");
}

void Config::Save()
{
  LOG_INFO("Saving settings");
  prefs_.putInt(N(Version), Version);
  prefs_.putLong(N(LoraFreqRx), LoraFreqRx);
  prefs_.putLong(N(LoraFreqTx), LoraFreqTx);
  prefs_.putLong(N(LoraBw), LoraBw);
  prefs_.putInt(N(LoraSf), LoraSf);
  prefs_.putInt(N(LoraCodingRate), LoraCodingRate);
  prefs_.putInt(N(LoraPower), LoraPower);
  prefs_.putInt(N(AudioCodec2Mode), AudioCodec2Mode);
  prefs_.putInt(N(AudioVol), AudioVol);
  prefs_.putInt(N(BatteryMonCal), BatteryMonCal);
  prefs_.putInt(N(PmLightSleepAfterMs), PmLightSleepAfterMs);
  LOG_INFO("Saved settings");
}

} // LoraDv