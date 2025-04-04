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

using namespace std;

namespace LoraDv {

class RadioTask;

class AudioTask {

public:
  explicit AudioTask(shared_ptr<const Config> config, shared_ptr<PmService> pmService);

  void start(shared_ptr<RadioTask> radioTask);
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
  constexpr static int CfgCoreId = 0;                        // core id where task will run
  constexpr static int CfgTaskPriority = 2;                  // task priority

  constexpr static i2s_port_t CfgAudioI2sSpkId = I2S_NUM_0;  // audio i2s speaker number
  constexpr static i2s_port_t CfgAudioI2sMicId = I2S_NUM_1;  // audio i2s mic number

  constexpr static uint32_t CfgAudioPlayBit = 0x01;          // task bit for playback
  constexpr static uint32_t CfgAudioRecBit = 0x02;           // task bit for recording

  constexpr static int CfgStartupDelayMs = 3000;             // startup delay
  constexpr static int CfgAudioTaskStack = 32768;            // audio stack size
  constexpr static int CfgPlayCompletedDelayMs = 500;        // playback stopped status after ms

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
  shared_ptr<const Config> config_;
  TaskHandle_t audioTaskHandle_;

  shared_ptr<RadioTask> radioTask_;
  shared_ptr<PmService> pmService_;

  Timer<1> playTimer_;
  Timer<1>::Task playTimerTask_;

  shared_ptr<Dsp> dsp_;
  shared_ptr<AudioCodec> audioCodec_;

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