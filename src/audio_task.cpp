#include "audio_task.h"

namespace LoraDv {

AudioTask::AudioTask()
  : isPttOn_(false)
{
}

void AudioTask::setup(const Config &config, std::shared_ptr<RadioTask> radioTask, std::shared_ptr<PmService> pmService)
{
  config_ = config;
  radioTask_ = radioTask;
  pmService_ = pmService;
  xTaskCreate(&audioTask, "audio_task", CfgAudioTaskStack, this, 5, &audioTaskHandle_);
}

void AudioTask::setupAudio(int bytesPerSample) 
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

void AudioTask::notifyPlay() 
{
  xTaskNotify(audioTaskHandle_, CfgAudioPlayBit, eSetBits);
}

void AudioTask::notifyRecord() 
{
  xTaskNotify(audioTaskHandle_, CfgAudioRecBit, eSetBits);
}

void AudioTask::audioTask(void *param) {
  static_cast<AudioTask*>(param)->audioPlayRecord();
}

void AudioTask::audioPlayRecord()
{
  LOG_INFO("Audio task started");

  // construct codec2
  codec_ = codec2_create(config_.AudioCodec2Mode);
  if (codec_ == NULL) {
    LOG_ERROR("Failed to create codec2");
    return;
  }
  codecSamplesPerFrame_ = codec2_samples_per_frame(codec_);
  codecBytesPerFrame_ = codec2_bytes_per_frame(codec_);
  codecSamples_ = (int16_t*)malloc(sizeof(int16_t) * codecSamplesPerFrame_);
  codecBits_ = (uint8_t*)malloc(sizeof(uint8_t) * codecBytesPerFrame_);
  LOG_INFO("C2 initialized", config_.AudioCodec2Mode, codecSamplesPerFrame_, codecBytesPerFrame_);
  delay(3000);
  setupAudio(codecSamplesPerFrame_);

  while(true) {
    uint32_t audioBits = 0;
    xTaskNotifyWaitIndexed(0, 0x00, ULONG_MAX, &audioBits, portMAX_DELAY);

    LOG_DEBUG("Audio task bits", audioBits);
    if (audioBits & CfgAudioPlayBit) {
      audioPlay();
    } else if (audioBits & CfgAudioRecBit) {
      audioRecord();
    }
  }
}

void AudioTask::audioPlay()
{
  size_t bytesWritten;
  LOG_DEBUG("Playing audio");
  double vol = (double)codecVolume_ / (double)CfgAudioMaxVolume;
  LOG_DEBUG("Volume is", vol);
  while (!isPttOn_ && radioTask_->hasData()) {
    byte packetSize;
    if (!radioTask_->readPacketSize(packetSize)) {
      LOG_ERROR("Failed to read packet size");
      vTaskDelay(1);
      continue;
    }
    pmService_->lightSleepReset();
    LOG_DEBUG("Playing packet", packetSize);
    // split by frame, decode and play
    for (int i = 0; i < packetSize; i++) {
      byte b;
      if (!radioTask_->readNextByte(b)) {
        LOG_ERROR("Failed to read next byte");
        vTaskDelay(1);
        continue;
      }
      codecBits_[i % codecBytesPerFrame_] = b;
      if (i % codecBytesPerFrame_ == codecBytesPerFrame_ - 1) {
        codec2_decode(codec_, codecSamples_, codecBits_);
        for (int j = 0; j < codecSamplesPerFrame_; j++) {
          codecSamples_[j] *= vol;
        }
        i2s_write(CfgAudioI2sSpkId, codecSamples_, sizeof(uint16_t) * codecSamplesPerFrame_, &bytesWritten, portMAX_DELAY);
        vTaskDelay(1);
      }
    }
  } // while rx data available
}

void AudioTask::audioRecord()
{      
  size_t bytesRead;
  LOG_DEBUG("Recording audio");
  int packetSize = 0;
  // record while button is pressed
  i2s_start(CfgAudioI2sMicId);
  while (isPttOn_) {
    // send packet if enough audio encoded frames are accumulated
    if (packetSize + codecBytesPerFrame_ > config_.AudioMaxPktSize) {
      LOG_DEBUG("Recorded packet", packetSize);
      if (!radioTask_->writePacketSize(packetSize)) {
        LOG_ERROR("Failed to write packet size");
        vTaskDelay(1);
        continue;
      }
      radioTask_->notifyTx();
      pmService_->lightSleepReset();
      packetSize = 0;
    }
    // read and encode one sample
    if (!isPttOn_) break;
    size_t bytesRead;
    i2s_read(CfgAudioI2sMicId, codecSamples_, sizeof(uint16_t) * codecSamplesPerFrame_, &bytesRead, portMAX_DELAY);
    if (!isPttOn_) break;
    codec2_encode(codec_, codecBits_, codecSamples_);
    if (!isPttOn_) break;
    for (int i = 0; i < codecBytesPerFrame_; i++) {
      radioTask_->writeNextByte(codecBits_[i]);
    }
    packetSize += codecBytesPerFrame_;
    vTaskDelay(1);
  } // btn_pressed_
  // send remaining tail audio encoded samples
  if (packetSize > 0) {
      LOG_DEBUG("Recorded packet", packetSize);
      if (radioTask_->writePacketSize(packetSize)) {
        radioTask_->notifyTx();
        pmService_->lightSleepReset();
      } else {
        LOG_ERROR("Failed to write byte");
      }
      packetSize = 0;
  }
  vTaskDelay(1);
  i2s_stop(CfgAudioI2sMicId);
}

} // LoraDv