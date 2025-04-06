#include "audio/audio_task.h"

#include "audio/audio_codec_codec2.h"
#include "audio/audio_codec_opus.h"

namespace LoraDv {

AudioTask::AudioTask(std::shared_ptr<const Config> config, std::shared_ptr<PmService> pmService)
  : config_(config)
  , audioTaskHandle_(0)
  , radioTask_(nullptr)
  , pmService_(pmService)
  , dsp_(make_shared<Dsp>(config->AudioHpfCutoffHz_, config->AudioSampleRate_))
  , audioCodec_(nullptr)
  , pcmResampleBuffer_(0)
  , pcmFrameBuffer_(0)
  , encodedFrameBuffer_(0)
  , codecSamplesPerFrame_(0)
  , codecBytesPerFrame_(0)
  , volume_(config->AudioVol)
  , maxVolume_(config->AudioMaxVol_)
  , isPttOn_(false)
  , isRunning_(false)
  , shouldUpdateScreen_(false)
  , isPlaying_(false)
  , playTimerTask_(0)
{
}

void AudioTask::start(std::shared_ptr<RadioTask> radioTask)
{
  radioTask_ = radioTask;
  xTaskCreatePinnedToCore(&task, "AudioTask", CfgAudioTaskStack, this, CfgTaskPriority, &audioTaskHandle_, CfgCoreId);
}

void AudioTask::changeVolume(int deltaVolume) 
{
  int newVolume = volume_ + deltaVolume;
  if (newVolume >= 0 && newVolume <= maxVolume_)
    setVolume(newVolume);
}

void AudioTask::installAudio(int bytesPerSample) const
{
  // speaker
  i2s_config_t i2sSpeakerConfig = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = config_->AudioSampleRate_,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = bytesPerSample,
    .use_apll = false,
    .tx_desc_auto_clear = true, 
    .fixed_mclk = -1    
  };
  i2s_pin_config_t i2sSpeakerPinConfig = {
    .bck_io_num = config_->AudioSpkPinBclk_,
    .ws_io_num = config_->AudioSpkPinLrc_,
    .data_out_num = config_->AudioSpkPinDin_,
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
    .sample_rate = config_->AudioSampleRate_,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = bytesPerSample,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = -1
  };
  i2s_pin_config_t i2sMicPinConfig = {
    .bck_io_num = config_->AudioMicPinSck_,
    .ws_io_num = config_->AudioMicPinWs_,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = config_->AudioMicPinSd_ 
  };
  if (i2s_driver_install(CfgAudioI2sMicId, &i2sMicConfig, 0, NULL) != ESP_OK) {
    LOG_ERROR("Failed to install i2s mic driver");
  }
  if (i2s_set_pin(CfgAudioI2sMicId, &i2sMicPinConfig) != ESP_OK) {
    LOG_ERROR("Failed to set i2s mic pins");
  }
}

void AudioTask::uninstallAudio() const
{
  i2s_stop(CfgAudioI2sSpkId);
  i2s_stop(CfgAudioI2sMicId);
  i2s_driver_uninstall(CfgAudioI2sSpkId);
  i2s_driver_uninstall(CfgAudioI2sMicId);
}

void AudioTask::playTimerReset()
{
  isPlaying_ = true;
  shouldUpdateScreen_ = true;
  if (playTimerTask_ != 0) {
    playTimer_.cancel(playTimerTask_);
  }
  playTimerTask_ = playTimer_.in(CfgPlayCompletedDelayMs, playTimerEnter, this);
}

bool AudioTask::playTimerEnter(void *param)
{
  static_cast<AudioTask*>(param)->playTimer();
  return false;
}

void AudioTask::playTimer()
{
  isPlaying_ = false;
  shouldUpdateScreen_ = true;
}

bool AudioTask::loop() 
{
  playTimer_.tick();
  bool shouldUpdateScreen = shouldUpdateScreen_;
  shouldUpdateScreen_ = false;
  return shouldUpdateScreen;
}

void AudioTask::setPtt(bool isPttOn) 
{
  isPttOn_ = isPttOn;
}

void AudioTask::play() const
{
  xTaskNotify(audioTaskHandle_, CfgAudioPlayBit, eSetBits);
}

void AudioTask::record() const
{
  radioTask_->startTransmit();
  xTaskNotify(audioTaskHandle_, CfgAudioRecBit, eSetBits);
}

void AudioTask::task(void *param) {
  static_cast<AudioTask*>(param)->audioTask();
}

void AudioTask::audioTask()
{
  LOG_INFO("Audio task started");
  isRunning_ = true;

  // select and codec
  if (config_->AudioCodec == CFG_AUDIO_CODEC_CODEC2)
    audioCodec_.reset(new AudioCodecCodec2());
  else if (config_->AudioCodec == CFG_AUDIO_CODEC_OPUS)
    audioCodec_.reset(new AudioCodecOpus());
  else {
    LOG_ERROR("Unknown codec", config_->AudioCodec);
    return;
  }

  audioCodec_->start(config_);

  // construct buffers
  codecSamplesPerFrame_ = audioCodec_->getPcmFrameSize();
  codecBytesPerFrame_ = audioCodec_->getFrameSize();
  pcmFrameBuffer_ = new int16_t[audioCodec_->getPcmFrameBufferSize()];
  pcmResampleBuffer_ = new int16_t[audioCodec_->getPcmFrameBufferSize() * config_->AudioResampleCoeff_];
  encodedFrameBuffer_ = new uint8_t[codecBytesPerFrame_];

  delay(CfgStartupDelayMs);
  installAudio(codecSamplesPerFrame_);

  while(isRunning_) {
    uint32_t audioBits = 0;
    xTaskNotifyWaitIndexed(0, 0x00, ULONG_MAX, &audioBits, portMAX_DELAY);

    LOG_DEBUG("Audio task command bits", audioBits);
    if (audioBits & CfgAudioPlayBit) {
      audioTaskPlay();
    } else if (audioBits & CfgAudioRecBit) {
      audioTaskRecord();
    }
  }

  delete encodedFrameBuffer_;
  delete pcmFrameBuffer_;
  audioCodec_->stop();

  uninstallAudio();

  LOG_INFO("Audio task stopped");
  vTaskDelete(NULL);
}

void AudioTask::audioTaskPlay()
{
  LOG_DEBUG("Playing audio");
  playTimerReset();

  int16_t targetLevel = dsp_->audioVolumeToLogPcm(volume_, maxVolume_, maxVolume_ * CfgAudioMaxVolumePcmMultiplier);
  LOG_DEBUG("Target level is", targetLevel);

  // run till ptt is not pressed and radio has data
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

      // read byte by byte from radio task
      byte b;
      if (!radioTask_->readNextByte(b)) {
        LOG_ERROR("Failed to read next byte");
        vTaskDelay(1);
        continue;
      }

      // split only if codec has fixed frame size, otherwise just process complete packet
      int subFrameSize = audioCodec_->isFixedFrameSize() ? codecBytesPerFrame_ : packetSize;
      encodedFrameBuffer_[i % subFrameSize] = b;

      // one encoded audio frame is read, decode and process
      if (i % subFrameSize == subFrameSize - 1) {

        // decode to pcm, adjust agc, upsample, and send for playback
        decodeAndPlay(subFrameSize, targetLevel);
        vTaskDelay(1);
      }
    }
  } // while rx data available
}

void AudioTask::decodeAndPlay(int frameSize, int16_t targetLevel)
{
  // decode in current codec
  int pcmFrameSize = audioCodec_->decode(pcmFrameBuffer_, encodedFrameBuffer_, frameSize);

  // adjust volume
  dsp_->audioAdjustGainAgc(pcmFrameBuffer_, pcmFrameSize, targetLevel);

  // upsample if codec rate is lower than speaker rate
  int writeDataSize = pcmFrameSize;
  int16_t* pcmBuffer = pcmFrameBuffer_;
  if (config_->AudioResampleCoeff_ == 2) {
    writeDataSize = dsp_->audioUpsample2x(pcmFrameBuffer_, pcmResampleBuffer_, pcmFrameSize);
    pcmBuffer = pcmResampleBuffer_;
  }

  // write to i2s speaker
  size_t bytesWritten;
  if (i2s_write(CfgAudioI2sSpkId, pcmBuffer, sizeof(int16_t) * writeDataSize, &bytesWritten, portMAX_DELAY) != ESP_OK) {
    LOG_ERROR("Failed to write to I2S speaker");
  }
}

void AudioTask::audioTaskRecord()
{      
  LOG_DEBUG("Recording audio");

  size_t bytesRead;
  int packetSize = 0;
  i2s_start(CfgAudioI2sMicId);

  // record while ptt button is pressed
  while (isPttOn_) {

    // transmit if enough audio frames aggregated for fixed frame codec (e.g. codec2)
    // or transmit immediately if variable size frame is read (e.g. OPUS)
    bool shouldTransmit = 
         (audioCodec_->isFixedFrameSize() && packetSize + codecBytesPerFrame_ > config_->AudioMaxPktSize) ||
         (!audioCodec_->isFixedFrameSize() && packetSize > 0);

    // perform packet transmission to radio
    if (shouldTransmit) {
      LOG_DEBUG("Recorded packet", packetSize);
      if (!radioTask_->writePacketSize(packetSize)) {
        LOG_ERROR("Failed to write packet size");
        vTaskDelay(1);
        continue;
      }
      radioTask_->transmit();
      pmService_->lightSleepReset();
      packetSize = 0;
    }

    // read one pcm sample from i2s microphone
    int readDataSize = codecSamplesPerFrame_ * config_->AudioResampleCoeff_;
    if (i2s_read(CfgAudioI2sMicId, pcmResampleBuffer_, sizeof(uint16_t) * readDataSize, &bytesRead, portMAX_DELAY) != ESP_OK) {
      LOG_ERROR("Failed to read from I2S microphone");
      continue;
    }

    // process pcm frame, apply filter, downsample and encode in selected codec, queue for transmission
    int encodedFrameSize = encodeAndQueue(readDataSize);
    packetSize += encodedFrameSize;

    vTaskDelay(1);
  } // while ptt pressed

  // send remaining tail audio encoded samples if any
  if (packetSize > 0) {
      LOG_DEBUG("Recorded packet tail", packetSize);
      if (radioTask_->writePacketSize(packetSize)) {
        radioTask_->transmit();
        pmService_->lightSleepReset();
      } else {
        LOG_ERROR("Failed to write byte");
      }
      packetSize = 0;
  }

  // stop mic and tell radio to start transmission
  vTaskDelay(1);
  i2s_stop(CfgAudioI2sMicId);
  radioTask_->startReceive();
}

int AudioTask::encodeAndQueue(int pcmFrameSize)
{
  int16_t *pcmReadBuffer = pcmResampleBuffer_;

  // apply high pass filter
  dsp_->audioFilterHpf(pcmReadBuffer, pcmFrameSize);

  // downsample if mic sample rate is higher than codec rate
  if (config_->AudioResampleCoeff_ == 2) {
    dsp_->audioDownsample2x(pcmReadBuffer, pcmFrameBuffer_, pcmFrameSize);
    pcmReadBuffer = pcmFrameBuffer_;
  }

  // encode in selected codec
  int encodedFrameSize = audioCodec_->encode(encodedFrameBuffer_, pcmReadBuffer);
  
  // transfer data to the radio packet queue without actual transmission
  for (int i = 0; i < encodedFrameSize; i++) {
    radioTask_->writeNextByte(encodedFrameBuffer_[i]);
  }
  return encodedFrameSize;
}

} // LoraDv