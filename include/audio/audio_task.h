#ifndef AUDIO_TASK_H
#define AUDIO_TASK_H

#include <driver/i2s.h>
#include <memory>
#include <codec2.h>

#include "hal/radio_task.h"
#include "settings/config.h"
#include "hal/pm_service.h"
#include "audio/audio_codec.h"
#include "utils/dsp.h"

namespace LoraDv {

class RadioTask;

class AudioTask {

public:
  explicit AudioTask(std::shared_ptr<const Config> config, std::shared_ptr<PmService> pmService);

  void start(std::shared_ptr<RadioTask> radioTask);
  inline void stop() { isRunning_ = false; }
  bool loop();

  void play() const; 
  bool isPlaying() const { return isPlaying_; }
  void record() const;

  void setPtt(bool isPttOn);

  inline void setVolume(int volume) { 
    if (volume >= 0 && volume <= maxVolume_) {
      volume_ = volume; 
    }
  }
  void changeVolume(int deltaVolume);
  inline int getVolume() const { return volume_; }

private:
  static constexpr int CfgCoreId = 0;                        // core id where task will run
  static constexpr int CfgTaskPriority = 2;                  // task priority

  static constexpr i2s_port_t CfgAudioI2sSpkId = I2S_NUM_0;  // audio i2s speaker number
  static constexpr i2s_port_t CfgAudioI2sMicId = I2S_NUM_1;  // audio i2s mic number

  static constexpr uint32_t CfgAudioPlayBit = 0x01;          // task bit for playback
  static constexpr uint32_t CfgAudioRecBit = 0x02;           // task bit for recording

  static constexpr int CfgStartupDelayMs = 3000;             // startup delay
  static constexpr int CfgAudioTaskStack = 32768;            // audio stack size
  static constexpr int CfgPlayCompletedDelayMs = 500;        // playback stopped status after ms
  static constexpr int CfgAudioMaxVolumePcmMultiplier = 10;  // multipier to get max pcm volume from max control volume

private:
  void installAudio(int bytesPerSample) const;
  void uninstallAudio() const;

  static void task(void *param);

  void audioTask();
  void audioTaskPlay();
  void audioTaskRecord();

  void decodeAndPlay(int frameSize, int16_t targetLevel);
  int encodeAndQueue(int pcmFrameSize);

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

  std::shared_ptr<Dsp> dsp_;
  std::shared_ptr<AudioCodec> audioCodec_;

  int16_t *pcmFrameBuffer_;
  int16_t *pcmResampleBuffer_;
  uint8_t *encodedFrameBuffer_;

  int codecSamplesPerFrame_;
  int codecBytesPerFrame_;

  long volume_;
  long maxVolume_;

  volatile bool isPttOn_;
  volatile bool isRunning_;
  volatile bool shouldUpdateScreen_;
  volatile bool isPlaying_;
};

}

#endif // RADIO_TASK_H