#include "utils/utils.h"

namespace LoraDv {

float Utils::loraGetSnrLimit(int sf, long bw) 
{
  float snrLimit = -7;
  switch (sf) {
    case 7:
        snrLimit = -7.5;
        break;
    case 8:
        snrLimit = -10.0;
        break;
    case 9:
        snrLimit = -12.6;
        break;
    case 10:
        snrLimit = -15.0;
        break;
    case 11:
        snrLimit = -17.5;
        break;
    case 12:
        snrLimit = -20.0;
        break;
  }
  return -174 + 10 * log10(bw) + 6 + snrLimit;
}

} // LoraDv