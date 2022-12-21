#ifndef AUDIO_TASK_H
#define AUDIO_TASK_H

#include <driver/i2s.h>
#include <memory>
#include <codec2.h>

#include "radio_task.h"
#include "loradv_config.h"
#include "pm_service.h"

namespace LoraDv {

class RadioTask;

class AudioTask {

public:
  AudioTask();

  void start(std::shared_ptr<Config> config, std::shared_ptr<RadioTask> radioTask, std::shared_ptr<PmService> pmService);
  inline void stop() { isRunning_ = false; }

  void play() const; 
  void record() const;

  inline void setPtt(bool isPttOn) { isPttOn_ = isPttOn; }

  inline void setVolume(int volume) { codecVolume_ = volume; }
  inline int getVolume() const { return codecVolume_; }
  inline int getMaxVolume() const { return CfgAudioMaxVolume; }

private:
  const uint32_t CfgAudioSampleRate = 8000;       // audio sample rate
  const i2s_port_t CfgAudioI2sSpkId = I2S_NUM_0;  // audio i2s speaker number
  const i2s_port_t CfgAudioI2sMicId = I2S_NUM_1;  // audio i2s mic number
  const int CfgAudioMaxVolume = 100;              // maximum volume value

  const uint32_t CfgAudioPlayBit = 0x01;          // task bit for playback
  const uint32_t CfgAudioRecBit = 0x02;           // task bit for recording

  const int CfgAudioTaskStack = 32768;            // audio stack size

private:
  void installAudio(int bytesPerSample) const;
  void uninstallAudio() const;

  static void task(void *param);

  void audioTask();
  void audioTaskPlay();
  void audioTaskRecord();

private:
  std::shared_ptr<Config> config_;
  TaskHandle_t audioTaskHandle_;

  std::shared_ptr<RadioTask> radioTask_;
  std::shared_ptr<PmService> pmService_;

  struct CODEC2 *codec_; 
  int16_t *codecSamples_;
  uint8_t *codecBits_;

  int codecSamplesPerFrame_;
  int codecBytesPerFrame_;
  long codecVolume_;

  bool isPttOn_;
  volatile bool isRunning_;
};

}

#endif // RADIO_TASK_H