#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

namespace LoraDv {

class Utils {

public:
  static float loraGetSnrLimit(int sf, long bw);
  static int loraGetSpeed(int sf, int cr, long bw) { return (int)(sf * (4.0 / cr) / (pow(2.0, sf) / bw)); }

  static int audioDownsample2x(int16_t *input, int16_t *output, int inputSize);
  static int audioUpsample2x(int16_t* input, int16_t* output, int inputSize);
  static void audioAdjustGain(int16_t* input, int inputSize, double gain);
  static void audioAdjustGainAgc(int16_t* input, int inputSize, int16_t targetLevel);

private:
  static constexpr double CfgAgcMaxGain = 20.0;
  static constexpr double CfgAgcMinGain = 0.1;
  static constexpr double CfgAgcStep = 0.1;
};

} // LoraDv

#endif // UTILS_H