#include "loradv_service.h"

namespace LoraDv {

std::shared_ptr<AiEsp32RotaryEncoder> Service::rotaryEncoder_;

Service::Service()
  : btnPressed_(false)
  , codecVolume_(CfgAudioMaxVolume)
{
}

void Service::setup(const Config &config)
{
  config_ = config;

  // setup logging
  LOG_SET_LEVEL(config_.LogLevel);
  LOG_SET_OPTION(false, false, true);  // disable file, line, enable func
  
  // rotary encoder
  LOG_INFO("Encoder setup started");
  rotaryEncoder_ = std::make_shared<AiEsp32RotaryEncoder>(config_.EncoderPinA, config_.EncoderPinB, 
    config_.EncoderPinBtn, config_.EncoderPinVcc, config_.EncoderSteps);
  rotaryEncoder_->begin();
  rotaryEncoder_->setBoundaries(0, CfgAudioMaxVolume);
  rotaryEncoder_->setEncoderValue(CfgAudioMaxVolume);
  rotaryEncoder_->setup(isrReadEncoder);
  LOG_INFO("Encoder setup completed");

  // oled screen
  display_ = std::make_shared<Adafruit_SSD1306>(CfgDisplayWidth, CfgDisplayHeight, &Wire, -1);
  if(display_->begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    LOG_INFO("Display setup completed");
  } else {
    LOG_ERROR("Display init failed");
  }

  // ptt button
  LOG_INFO("PTT setup started");
  pinMode(config_.PttBtnPin, INPUT);
  LOG_INFO("PTT setup completed");

  // start codec2 playback task
  xTaskCreate(&audioTask, "audio_task", CfgAudioTaskStack, this, 5, &audioTaskHandle_);

  // start lora task
  radioTask_.setup(config, audioTaskHandle_, CfgAudioPlayBit);

  // sleep
  LOG_INFO("Light sleep is enabled");
  lightSleepReset();
  printStatus("RX");

  LOG_INFO("Board setup completed");
}

void Service::setupAudio(int bytesPerSample) 
{
  // speaker
  i2s_config_t i2sSpeakerConfig = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = CfgAudioSampleRate,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = bytesPerSample,
    .use_apll=0,
    .tx_desc_auto_clear= true, 
    .fixed_mclk=-1    
  };
  i2s_pin_config_t i2sSpeakerPinConfig = {
    .bck_io_num = config_.AudioSpkPinBclk,
    .ws_io_num = config_.AudioSpkPinLrc,
    .data_out_num = config_.AudioSpkPinDin,
    .data_in_num = I2S_PIN_NO_CHANGE
  };
  if (i2s_driver_install(CfgAudioI2sSpkId, &i2sSpeakerConfig, 0, NULL) != ESP_OK) {
    LOG_ERROR("Failed to install i2s speaker driver");
  }
  if (i2s_set_pin(CfgAudioI2sSpkId, &i2sSpeakerPinConfig) != ESP_OK) {
    LOG_ERROR("Failed to set i2s speaker pins");
  }
  // mic
  i2s_config_t i2sMicConfig = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = CfgAudioSampleRate,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = bytesPerSample,
    .use_apll=0,
    .tx_desc_auto_clear= true,
    .fixed_mclk=-1
  };
  i2s_pin_config_t i2sMicPinConfig = {
    .bck_io_num = config_.AudioMicPinSck,
    .ws_io_num = config_.AudioMicPinWs,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = config_.AudioMicPinSd 
  };
  if (i2s_driver_install(CfgAudioI2sMicId, &i2sMicConfig, 0, NULL) != ESP_OK) {
    LOG_ERROR("Failed to install i2s mic driver");
  }
  if (i2s_set_pin(CfgAudioI2sMicId, &i2sMicPinConfig) != ESP_OK) {
    LOG_ERROR("Failed to set i2s mic pins");
  }
}

IRAM_ATTR void Service::isrReadEncoder()
{
  rotaryEncoder_->readEncoder_ISR();
}

float Service::getBatteryVoltage() 
{
  int bat_value = analogRead(config_.BatteryMonPin);
  return 2 * bat_value * (3.3 / 4096.0) + config_.BatteryMonCal;
}

void Service::printStatus(const String &str)
{
    display_->clearDisplay();
    display_->setTextSize(2);
    display_->setTextColor(WHITE);
    display_->setCursor(0, 0);
    display_->print(str); display_->print(" "); 
    if (btnPressed_)
      display_->println((float)config_.LoraFreqTx / 1e6);
    else
      display_->println((float)config_.LoraFreqRx / 1e6);
    display_->print(codecVolume_); display_->print("% "); display_->print(getBatteryVoltage()); display_->print("V");
    display_->display();
}

void Service::lightSleepReset() {
  LOG_DEBUG("Reset light sleep");
  if (lightSleepTimerTask_ != NULL) lightSleepTimer_.cancel(lightSleepTimerTask_);
  lightSleepTimerTask_ = lightSleepTimer_.in(config_.PmLightSleepAfterMs, lightSleepEnterTimer, this);
}

bool Service::lightSleepEnterTimer(void *param) {
  static_cast<Service*>(param)->lightSleepEnter();
  return false;
}

void Service::lightSleepEnter(void) {
  LOG_INFO("Entering light sleep");
  display_->clearDisplay();
  display_->display();

  esp_sleep_wakeup_cause_t wakeupCause = ESP_SLEEP_WAKEUP_UNDEFINED;
  while (true) {
    wakeupCause = lightSleepWait(config_.PmLightSleepDurationMs * 1000UL);
    if (wakeupCause != ESP_SLEEP_WAKEUP_TIMER) break;
    delay(config_.PmLightSleepAwakeMs);
  }

  LOG_INFO("Exiting light sleep");
  lightSleepReset();
  printStatus("RX");
}

esp_sleep_wakeup_cause_t Service::lightSleepWait(uint64_t sleepTimeUs) {
  esp_sleep_enable_ext0_wakeup(config_.PttBtnGpioPin, LOW);
  uint64_t bitMask = (uint64_t)(1 << config_.LoraPinA) | (uint64_t)(1 << config_.LoraPinB);
  esp_sleep_enable_ext1_wakeup(bitMask, ESP_EXT1_WAKEUP_ANY_HIGH);
  esp_sleep_enable_timer_wakeup(sleepTimeUs);
  esp_light_sleep_start();
  return esp_sleep_get_wakeup_cause();
}

void Service::audioTask(void *param) {
  static_cast<Service*>(param)->audioPlayRecord();
}

void Service::audioPlayRecord()
{
  LOG_INFO("Audio task started");

  // construct codec2
  struct CODEC2 *codec = codec2_create(config_.AudioCodec2Mode);
  if (codec == NULL) {
    LOG_ERROR("Failed to create codec2");
    return;
  }
  int codecSamplesPerFrame = codec2_samples_per_frame(codec);
  codecBytesPerFrame_ = codec2_bytes_per_frame(codec);
  int16_t *codecSamples = (int16_t*)malloc(sizeof(int16_t) * codecSamplesPerFrame);
  uint8_t *codecBits = (uint8_t*)malloc(sizeof(uint8_t) * codecBytesPerFrame_);
  LOG_INFO("C2 initialized", codecSamplesPerFrame, codecBytesPerFrame_);
  delay(3000);
  setupAudio(codecSamplesPerFrame);

  // wait for data notification, decode frames and playback
  size_t bytesRead, bytesWritten;
  while(true) {
    uint32_t audioBits = 0;
    xTaskNotifyWaitIndexed(0, 0x00, ULONG_MAX, &audioBits, portMAX_DELAY);

    LOG_DEBUG("Audio task bits", audioBits);

    // audio rx-decode-playback
    if (audioBits & CfgAudioPlayBit) {
      LOG_DEBUG("Playing audio");
      double vol = (double)codecVolume_ / (double)CfgAudioMaxVolume;
      LOG_DEBUG("Volume is", vol);
      // while rx frames are available and button is not pressed
      while (!btnPressed_ && radioTask_.hasData()) {
        byte packetSize;
        if (!radioTask_.readPacketSize(packetSize)) {
          LOG_ERROR("Failed to read packet size");
          vTaskDelay(1);
          continue;
        }
        LOG_DEBUG("Playing packet", packetSize);
        // split by frame, decode and play
        for (int i = 0; i < packetSize; i++) {
          byte b;
          if (!radioTask_.readNextByte(b)) {
            LOG_ERROR("Failed to read next byte");
            vTaskDelay(1);
            continue;
          }
          codecBits[i % codecBytesPerFrame_] = b;
          if (i % codecBytesPerFrame_ == codecBytesPerFrame_ - 1) {
            codec2_decode(codec, codecSamples, codecBits);
            for (int j = 0; j < codecSamplesPerFrame; j++) {
              codecSamples[j] *= vol;
            }
            i2s_write(CfgAudioI2sSpkId, codecSamples, sizeof(uint16_t) * codecSamplesPerFrame, &bytesWritten, portMAX_DELAY);
            vTaskDelay(1);
            lightSleepReset();
          }
        }
      } // while rx data available
    } // audio decode playback
    // audio record-encode-tx
    else if (audioBits & CfgAudioRecBit) {
      LOG_DEBUG("Recording audio");
      int packetSize = 0;
      // record while button is pressed
      i2s_start(CfgAudioI2sMicId);
      while (btnPressed_) {
        // send packet if enough audio encoded frames are accumulated
        if (packetSize + codecBytesPerFrame_ > config_.AudioMaxPktSize) {
          LOG_DEBUG("Recorded packet", packetSize);
          if (!radioTask_.writePacketSize(packetSize)) {
            LOG_ERROR("Failed to write packet size");
            vTaskDelay(1);
            continue;
          }
          radioTask_.notifyTx();
          packetSize = 0;
        }
        // read and encode one sample
        if (!btnPressed_) break;
        size_t bytesRead;
        lightSleepReset();
        i2s_read(CfgAudioI2sMicId, codecSamples, sizeof(uint16_t) * codecSamplesPerFrame, &bytesRead, portMAX_DELAY);
        if (!btnPressed_) break;
        codec2_encode(codec, codecBits, codecSamples);
        if (!btnPressed_) break;
        for (int i = 0; i < codecBytesPerFrame_; i++) {
          radioTask_.writeNextByte(codecBits[i]);
        }
        packetSize += codecBytesPerFrame_;
        vTaskDelay(1);
      } // btn_pressed_
      // send remaining tail audio encoded samples
      if (packetSize > 0) {
          LOG_DEBUG("Recorded packet", packetSize);
          if (radioTask_.writePacketSize(packetSize)) {
            radioTask_.notifyTx();
          } else {
            LOG_ERROR("Failed to write byte");
          }
          packetSize = 0;
      }
      vTaskDelay(1);
      i2s_stop(CfgAudioI2sMicId);
    } // task bit
  }
}

void Service::loop() 
{
  // button 
  if (digitalRead(config_.PttBtnPin) == LOW && !btnPressed_) {
    LOG_DEBUG("PTT pushed, start TX");
    btnPressed_ = true;
    printStatus("TX");
    // notify to start recording
    uint32_t audio_bits = CfgAudioRecBit;
    xTaskNotify(audioTaskHandle_, audio_bits, eSetBits);
  } else if (digitalRead(config_.PttBtnPin) == HIGH && btnPressed_) {
    LOG_DEBUG("PTT released");
    printStatus("RX");
    btnPressed_ = false;
  }
  // rotary encoder
  if (rotaryEncoder_->encoderChanged())
  {
    LOG_INFO("Encoder changed:", rotaryEncoder_->readEncoder());
    codecVolume_ = rotaryEncoder_->readEncoder();
    printStatus("RX");
    lightSleepReset();
  }
  if (rotaryEncoder_->isEncoderButtonClicked())
  {
    LOG_INFO("Encoder button pressed", esp_get_free_heap_size());
    lightSleepReset();
  }
  if (rotaryEncoder_->isEncoderButtonClicked(2000))
  {
    LOG_INFO("Encoder button long pressed");
    lightSleepReset();
  }
  lightSleepTimer_.tick();
}

} // LoraDv