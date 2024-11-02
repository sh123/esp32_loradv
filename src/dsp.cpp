#include <math.h>
#include "dsp.h"

namespace LoraDv {

Dsp::Dsp(int hpfCutoffFreqHz, int hpfSampleRate) 
    : currentAgcGain_(1.0)
    , hpfX1_(0.0f)
    , hpfX2_(0.0f)
    , hpfY1_(0.0f)
    , hpfY2_(0.0f)
{
    // initialize hpf coefficients
    float omega = 2.0f * M_PI * hpfCutoffFreqHz / hpfSampleRate;
    float alpha = sinf(omega) / (1.0f + cosf(omega));

    hpfA0_ = (1.0f + cosf(omega)) / 2.0f;
    hpfA1_ = -(1.0f + cosf(omega));
    hpfA2_ = (1.0f + cosf(omega)) / 2.0f;
    hpfB1_ = -cosf(omega);
    hpfB2_ = alpha;
}

int Dsp::audioDownsample2x(int16_t *pcmInput, int16_t *pcmOutput, int pcmInputSize)
{
  for (int i = 0; i < pcmInputSize / 2; i++)
  {
    pcmOutput[i] = pcmInput[i * 2] / 2 + pcmInput[(i * 2) + 1] / 2;
  }
  return pcmInputSize / 2;
}

int Dsp::audioUpsample2x(int16_t* pcmInput, int16_t* pcmOutput, int pcmInputSize)
{
  for (int i = 0; i < pcmInputSize; i++)
  {
    pcmOutput[2 * i] = pcmInput[i];
  }
  for (int i = 0; i < pcmInputSize - 1; i++)
  {
    pcmOutput[2 * i + 1] =  pcmOutput[2 * i] + (pcmOutput[2 * i + 2] - pcmOutput[2 * i]) / 2;
  }
  pcmOutput[2 * pcmInputSize - 1] = pcmInput[pcmInputSize - 1];
  return pcmInputSize * 2;
}

void Dsp::audioAdjustGain(int16_t* pcmBuffer, int pcmBufferSize, float gain) 
{
  for (int i = 0; i < pcmBufferSize; i++) 
  {
    pcmBuffer[i] *= gain;
  }
}

void Dsp::audioAdjustGainAgc(int16_t* pcmBuffer, int pcmBufferSize, int16_t targetLevel)
{
  for (int i = 0; i < pcmBufferSize; i++) 
  {
    int16_t sample = pcmBuffer[i];
    int16_t newSample = sample * currentAgcGain_;

    if (abs(newSample) > targetLevel) currentAgcGain_ -= CfgAgcStep;
    else currentAgcGain_ += CfgAgcStep;

    if (currentAgcGain_ < CfgAgcMinGain) currentAgcGain_ = CfgAgcMinGain;
    if (currentAgcGain_ > CfgAgcMaxGain) currentAgcGain_ = CfgAgcMaxGain;

    pcmBuffer[i] = newSample;
  }
}

void Dsp::audioFilterHpf(int16_t *pcmBuffer, int pcmBufferSize) 
{
  for (int i = 0; i < pcmBufferSize; i++) 
  {
    float x0 = (float)pcmBuffer[i];
    float y0 = hpfA0_ * x0 + hpfA1_ * hpfX1_ + hpfA2_ * hpfX2_ - hpfB1_ * hpfY1_ - hpfB2_ * hpfY2_;

    pcmBuffer[i] = (int16_t)y0;

    hpfX2_ = hpfX1_; hpfX1_ = x0; hpfY2_ = hpfY1_; hpfY1_ = y0;
  }
}

} // namespace LoraDv