#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

namespace LoraDv {

class Utils {

public:
  static float loraGetSnrLimit(int sf, long bw);
  static int loraGetSpeed(int sf, int cr, long bw) { return (int)(sf * (4.0 / cr) / (pow(2.0, sf) / bw)); }
};

} // LoraDv

#endif // UTILS_H