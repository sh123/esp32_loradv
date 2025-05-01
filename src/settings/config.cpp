#include "settings/config.h"

namespace LoraDv {

#define N(v) #v

CFG_AUDIO_PRIVACY_KEY;
CFG_AUDIO_PRIVACY_DATA;

Config::Config()
{
  InitializeDefault();
}

void Config::InitializeDefault()
{
  // default version
  Version = CFG_VERSION;
  
  // log level
  LogLevel = CFG_LOG_LEVEL;

  // modulation type
  ModType = CFG_MOD_TYPE;
  
  // generic parameters
  LoraFreqRx = CFG_LORA_FREQ_RX;
  LoraFreqTx = CFG_LORA_FREQ_TX;
  LoraFreqStep = CFG_LORA_FREQ_STEP;
  LoraPower = CFG_LORA_PWR;

  // lora parameters, must match on devices
  LoraBw = CFG_LORA_BW;
  LoraSf = CFG_LORA_SF;
  LoraCodingRate = CFG_LORA_CR;
  LoraSync_ = CFG_LORA_SYNC;
  LoraCrc_ = CFG_LORA_CRC; // set to 0 to disable
  LoraPreambleLen_ = CFG_LORA_PREAMBLE_LEN;

  // fsk parameters
  FskBitRate = CFG_FSK_BIT_RATE;
  FskFreqDev = CFG_FSK_FREQ_DEV;
  FskRxBw = CFG_FSK_RX_BW;
  FskShaping = CFG_FSK_SHAPING;

  // lora pinouts
  LoraPinSs_ = CFG_LORA_PIN_SS;
  LoraPinRst_ = CFG_LORA_PIN_RST;
  LoraPinA_ = CFG_LORA_PIN_A; // (sx127x - dio0, sx126x/sx128x - dio1)
  LoraPinB_ = CFG_LORA_PIN_B; // (sx127x - dio1, sx126x/sx128x - busy)
  LoraPinSwitchRx_ = CFG_LORA_PIN_RXEN;  // (sx127x - unused, sx126x - RXEN pin number)
  LoraPinSwitchTx_ = CFG_LORA_PIN_TXEN;  // (sx127x - unused, sx126x - TXEN pin number)

  // ptt button
  PttBtnPin_ = CFG_PTT_BTN_PIN;

  // encoder
  EncoderPinA_ = CFG_ENCODER_PIN_A;
  EncoderPinB_ = CFG_ENCODER_PIN_B;
  EncoderPinBtn_ = CFG_ENCODER_PIN_BTN;
  EncoderPinVcc_ = CFG_ENCODER_PIN_VCC;
  EncoderSteps_ = CFG_ENCODER_STEPS;

  // audio parameters
  AudioCodec = CFG_AUDIO_CODEC_CODEC2;
  AudioResampleCoeff_ = CFG_AUDIO_RESAMPLE_COEFF;
  AudioSampleRate_ = CFG_AUDIO_SAMPLE_RATE;
  AudioCodecSampleRate_ = CFG_AUDIO_CODEC_SAMPLE_RATE;
  AudioHpfCutoffHz_ = CFG_AUDIO_HPF_CUTOFF_HZ;
  AudioCodec2Mode = CFG_AUDIO_CODEC2_MODE;
  AudioMaxPktSize = CFG_AUDIO_MAX_PKT_SIZE;
  AudioMaxVol_ = CFG_AUDIO_MAX_VOL;
  AudioVol = CFG_AUDIO_VOL;
  AudioEnPriv = CFG_AUDIO_ENABLE_PRIVACY;

  // audio, opus
  AudioOpusRate = CFG_AUDIO_OPUS_BITRATE;
  AudioOpusPcmLen = CFG_AUDIO_OPUS_PCMLEN;

  // i2s speaker
  AudioSpkPinBclk_ = CFG_AUDIO_SPK_PIN_BCLK;
  AudioSpkPinLrc_ = CFG_AUDIO_SPK_PIN_LRC;
  AudioSpkPinDin_ = CFG_AUDIO_SPK_PIN_DIN;

  // i2s mic
  AudioMicPinSd_ = CFG_AUDIO_MIC_PIN_SD;
  AudioMicPinWs_ = CFG_AUDIO_MIC_PIN_WS;
  AudioMicPinSck_ = CFG_AUDIO_MIC_PIN_SCK;

  // battery monitor
  BatteryMonPin_ = CFG_AUDIO_BATTERY_MON_PIN;
  BatteryMonCal = CFG_AUDIO_BATTERY_MON_CAL;

  // power management
  PmSleepAfterMs = CFG_PM_LSLEEP_AFTER_MS;
  PmLightSleepDurationMs_ = CFG_PM_LSLEEP_DURATION_MS;
  PmLightSleepAwakeMs_ = CFG_PM_LSLEEP_AWAKE_MS;

  // encryption key and auth tag data
  memcpy(AudioPrivacyKey_, AudioPrivacyKey, sizeof(AudioPrivacyKey));
  memcpy(AudioPrivacyData_, AudioPrivacyData, sizeof(AudioPrivacyData));
}

void Config::Reset()
{
  InitializeDefault();
  Save();
}

void Config::Load()
{
  LOG_INFO("Loading settings");
  prefs_.begin("LoraDv");

  // uninitialized, save
  if (!prefs_.isKey(N(Version))) {
    LOG_INFO("Not loaded, first time settings");
    prefs_.end();
    Save();
    return;
  }
  // new default version is higher, save and use default settings
  int version = prefs_.getInt(N(Version));
  LOG_INFO("Current prefs version", version);
  LOG_INFO("New prefs version", Version);
  if (Version > version) {
    LOG_INFO("Not loaded, new default settings version requested");
    prefs_.end();
    Save();
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
  if (prefs_.isKey(N(LoraFreqStep))) {
    LoraFreqStep = prefs_.getLong(N(LoraFreqStep));
  } else {
    prefs_.putLong(N(LoraFreqStep), LoraFreqStep);
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
    if (AudioVol > CFG_AUDIO_MAX_VOL) {
      AudioVol = CFG_AUDIO_VOL;
      prefs_.putInt(N(AudioVol), AudioVol);
    }
  } else {
    prefs_.putInt(N(AudioVol), AudioVol);
  }
  if (prefs_.isKey(N(AudioMaxPktSize))) {
    AudioMaxPktSize = prefs_.getInt(N(AudioMaxPktSize));
  } else {
    prefs_.putInt(N(AudioMaxPktSize), AudioMaxPktSize);
  }
  if (prefs_.isKey(N(AudioEnPriv))) {
    AudioEnPriv = prefs_.getBool(N(AudioEnPriv));
  } else {
    prefs_.putBool(N(AudioEnPriv), AudioEnPriv);
  }
  if (prefs_.isKey(N(BatteryMonCal))) {
    BatteryMonCal = prefs_.getFloat(N(BatteryMonCal));
  } else {
    prefs_.putFloat(N(BatteryMonCal), BatteryMonCal);
  }
  if (prefs_.isKey(N(PmSleepAfterMs))) {
    PmSleepAfterMs = prefs_.getInt(N(PmSleepAfterMs));
  } else {
    prefs_.putInt(N(PmSleepAfterMs), PmSleepAfterMs);
  } 
   if (prefs_.isKey(N(FskBitRate))) {
    FskBitRate = prefs_.getFloat(N(FskBitRate));
  } else {
    prefs_.putFloat(N(FskBitRate), FskBitRate);
  }
  if (prefs_.isKey(N(FskFreqDev))) {
    FskFreqDev = prefs_.getFloat(N(FskFreqDev));
  } else {
    prefs_.putFloat(N(FskFreqDev), FskFreqDev);
  }
  if (prefs_.isKey(N(FskRxBw))) {
    FskRxBw = prefs_.getFloat(N(FskRxBw));
  } else {
    prefs_.putFloat(N(FskRxBw), FskRxBw);
  }
  if (prefs_.isKey(N(FskShaping))) {
    FskShaping = prefs_.getInt(N(FskShaping));
  } else {
    prefs_.putInt(N(FskShaping), FskShaping);
  }
  if (prefs_.isKey(N(ModType))) {
    ModType = prefs_.getInt(N(ModType));
  } else {
    prefs_.putInt(N(ModType), ModType);
  } 
  if (prefs_.isKey(N(AudioOpusRate))) {
    AudioOpusRate = prefs_.getInt(N(AudioOpusRate));
  } else {
    prefs_.putInt(N(AudioOpusRate), AudioOpusRate);
  } 
  if (prefs_.isKey(N(AudioOpusPcmLen))) {
    AudioOpusPcmLen = prefs_.getInt(N(AudioOpusPcmLen));
  } else {
    prefs_.putInt(N(AudioOpusPcmLen), AudioOpusPcmLen);
  }
  if (prefs_.isKey(N(AudioCodec))) {
    AudioCodec = prefs_.getInt(N(AudioCodec));
  } else {
    prefs_.putInt(N(AudioCodec), AudioCodec);
  }

  prefs_.end();
  LOG_INFO("Settings are loaded");
}

void Config::Save()
{
  LOG_INFO("Saving settings");
  prefs_.begin("LoraDv");
  prefs_.putInt(N(Version), Version);
  prefs_.putLong(N(LoraFreqRx), LoraFreqRx);
  prefs_.putLong(N(LoraFreqTx), LoraFreqTx);
  prefs_.putLong(N(LoraFreqStep), LoraFreqStep);
  prefs_.putLong(N(LoraBw), LoraBw);
  prefs_.putInt(N(LoraSf), LoraSf);
  prefs_.putInt(N(LoraCodingRate), LoraCodingRate);
  prefs_.putInt(N(LoraPower), LoraPower);
  prefs_.putInt(N(AudioCodec2Mode), AudioCodec2Mode);
  prefs_.putInt(N(AudioVol), AudioVol);
  prefs_.putInt(N(AudioMaxPktSize), AudioMaxPktSize);
  prefs_.putBool(N(AudioEnPriv), AudioEnPriv);
  prefs_.putFloat(N(BatteryMonCal), BatteryMonCal);
  prefs_.putInt(N(PmSleepAfterMs), PmSleepAfterMs);
  prefs_.putFloat(N(FskBitRate), FskBitRate);
  prefs_.putFloat(N(FskFreqDev), FskFreqDev);
  prefs_.putFloat(N(FskRxBw), FskRxBw);
  prefs_.putInt(N(FskShaping), FskShaping);
  prefs_.putInt(N(ModType), ModType);
  prefs_.putInt(N(AudioOpusRate), AudioOpusRate);
  prefs_.putInt(N(AudioOpusPcmLen), AudioOpusPcmLen);
  prefs_.putInt(N(AudioCodec), AudioCodec);
  prefs_.end();
  LOG_INFO("Saved settings");
}

} // LoraDv