#include "loradv_service.h"

LoraDv::Service loraDvService_;

void initializeConfig(LoraDv::Config &cfg) 
{
  // log level
  cfg.LogLevel = CFG_LOG_LEVEL;
  
  // lora parameters, must match on devices
  cfg.LoraFreqRx = CFG_LORA_FREQ_RX;
  cfg.LoraFreqTx = CFG_LORA_FREQ_TX;
  cfg.LoraBw = CFG_LORA_BW;
  cfg.LoraSf = CFG_LORA_SF;
  cfg.LoraCodingRate = CFG_LORA_CR;
  cfg.LoraSync = CFG_LORA_SYNC;
  cfg.LoraCrc = CFG_LORA_CRC; // set to 0 to disable
  cfg.LoraExplicit = CFG_LORA_EXPLICIT;
  cfg.LoraPower = CFG_LORA_PWR;

  // lora pinouts
  cfg.LoraPinSs = CFG_LORA_PIN_SS;
  cfg.LoraPinRst = CFG_LORA_PIN_RST;
  cfg.LoraPinA = CFG_LORA_PIN_A; // (sx127x - dio0, sx126x/sx128x - dio1)
  cfg.LoraPinB = CFG_LORA_PIN_B; // (sx127x - dio1, sx126x/sx128x - busy)
  cfg.LoraPinSwitchRx = CFG_LORA_PIN_RXEN;  // (sx127x - unused, sx126x - RXEN pin number)
  cfg.LoraPinSwitchTx = CFG_LORA_PIN_TXEN;  // (sx127x - unused, sx126x - TXEN pin number)

  // ptt button
  cfg.PttBtnPin = CFG_PTT_BTN_PIN;
  cfg.PttBtnGpioPin = CFG_PTT_BTN_GPIO_PIN;

  // encoder
  cfg.EncoderPinA = CFG_ENCODER_PIN_A;
  cfg.EncoderPinB = CFG_ENCODER_PIN_B;
  cfg.EncoderPinBtn = CFG_ENCODER_PIN_BTN;
  cfg.EncoderPinVcc = CFG_ENCODER_PIN_VCC;
  cfg.EncoderSteps = CFG_ENCODER_STEPS;

  // audio parameters
  cfg.AudioCodec2Mode = CFG_AUDIO_CODEC2_MODE;
  cfg.AudioMaxPktSize = CFG_AUDIO_MAX_PKT_SIZE;

  // i2s speaker
  cfg.AudioSpkPinBclk = CFG_AUDIO_SPK_PIN_BCLK;
  cfg.AudioSpkPinLrc = CFG_AUDIO_SPK_PIN_LRC;
  cfg.AudioSpkPinDin = CFG_AUDIO_SPK_PIN_DIN;

  // i2s mic
  cfg.AudioMicPinSd = CFG_AUDIO_MIC_PIN_SD;
  cfg.AudioMicPinWs = CFG_AUDIO_MIC_PIN_WS;
  cfg.AudioMicPinSck = CFG_AUDIO_MIC_PIN_SCK;

  // battery monitor
  cfg.BatteryMonPin = CFG_AUDIO_BATTERY_MON_PIN;
  cfg.BatteryMonCal = CFG_AUDIO_BATTERY_MON_CAL;

  // power management
  cfg.PmLightSleepAfterMs = CFG_PM_LSLEEP_AFTER_MS;
  cfg.PmLightSleepDurationMs = CFG_PM_LSLEEP_DURATION_MS;
  cfg.PmLightSleepAwakeMs = CFG_PM_LSLEEP_AWAKE_MS;
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  while (!Serial);

  LoraDv::Config config;
  initializeConfig(config);

  loraDvService_.setup(config);
}

void loop() {
  loraDvService_.loop();
  delay(50);
}

