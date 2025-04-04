#ifndef DSP_H
#define DSP_H

#include <Arduino.h>

namespace LoraDv {

class Dsp {

public:
  explicit Dsp(int hpfCutoffFreqHz, int hpfSampleRate);

  int audioDownsample2x(int16_t *pcmInput, int16_t *pcmOutput, int pcmInputSize);
  int audioUpsample2x(int16_t *pcmInput, int16_t *pcmOutput, int pcmInputSize);

  void audioAdjustGain(int16_t *pcmBuffer, int inputSize, float gain);
  void audioAdjustGainAgc(int16_t *pcmBuffer, int inputSize, int16_t targetLevel);

  void audioFilterHpf(int16_t *pcmBuffer, int pcmBufferSize);

private:
  // AGC configuration
  static constexpr float CfgAgcMaxGain = 20.0;
  static constexpr float CfgAgcMinGain = 0.1;
  static constexpr float CfgAgcStep = 0.1;

  // agc
  float currentAgcGain_;
  
  // high pass filter
  float hpfX1_, hpfX2_, hpfY1_, hpfY2_;
  float hpfA0_, hpfA1_, hpfA2_, hpfB1_, hpfB2_;      
};

} // namespace LoraDv

#endif // DSP_H
