#ifndef AUDIO_TASK_H
#define AUDIO_TASK_H

#include <driver/i2s.h>
#include <memory>
#include <codec2.h>

#include "radio_task.h"
#include "loradv_config.h"
#include "pm_service.h"
#include "audio_codec.h"

namespace LoraDv {

class RadioTask;

class AudioTask {

public:
  AudioTask();

  void start(std::shared_ptr<const Config> config, std::shared_ptr<RadioTask> radioTask, std::shared_ptr<PmService> pmService);
  inline void stop() { isRunning_ = false; }
  bool loop();

  void play() const; 
  bool isPlaying() const { return isPlaying_; }
  void record() const;

  void setPtt(bool isPttOn);

  inline void setVolume(int volume) { if (volume <= maxVolume_) volume_ = volume; }
  void changeVolume(int deltaVolume);
  inline int getVolume() const { return volume_; }

private:
  const i2s_port_t CfgAudioI2sSpkId = I2S_NUM_0;  // audio i2s speaker number
  const i2s_port_t CfgAudioI2sMicId = I2S_NUM_1;  // audio i2s mic number

  const uint32_t CfgAudioPlayBit = 0x01;          // task bit for playback
  const uint32_t CfgAudioRecBit = 0x02;           // task bit for recording

  const int CfgAudioTaskStack = 32768;            // audio stack size
  const int CfgPlayCompletedDelayMs = 500;        // playback stopped status after ms

private:
  void installAudio(int bytesPerSample) const;
  void uninstallAudio() const;

  static void task(void *param);

  void audioTask();
  void audioTaskPlay();
  void audioTaskRecord();

  void playTimerReset();
  static bool playTimerEnter(void *param);
  void playTimer();

private:
  std::shared_ptr<const Config> config_;
  TaskHandle_t audioTaskHandle_;

  std::shared_ptr<RadioTask> radioTask_;
  std::shared_ptr<PmService> pmService_;

  Timer<1> playTimer_;
  Timer<1>::Task playTimerTask_;

  std::shared_ptr<AudioCodec> audioCodec_;

  int16_t *pcmFrameBuffer_;
  uint8_t *encodedFrameBuffer_;

  int codecSamplesPerFrame_;
  int codecBytesPerFrame_;

  long volume_;
  long maxVolume_;

  bool isPttOn_;
  volatile bool isRunning_;
  volatile bool shouldUpdateScreen_;
  volatile bool isPlaying_;
};

}

#endif // RADIO_TASK_H