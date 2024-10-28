#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

namespace LoraDv {

class Utils {

public:
  static float getLoraSnrLimit(int sf, long bw);
  static int getLoraSpeed(int sf, int cr, long bw) { return (int)(sf * (4.0 / cr) / (pow(2.0, sf) / bw)); }

  static int audioDownsample2x(int16_t *input, int16_t *output, int input_size);
  static int audioUpsample2x(int16_t* input, int16_t* output, int input_size);
  static void audioAdjustGain(int16_t* input, int input_size, double gain);
};

} // LoraDv

#endif // UTILS_H