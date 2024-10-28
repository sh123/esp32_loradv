#include "utils.h"

namespace LoraDv {

float Utils::getLoraSnrLimit(int sf, long bw) 
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

int Utils::audio_downsample_2x(int16_t *input, int16_t *output, int input_size)
{
    for (int i = 0; i < input_size / 2; i++)
    {
        output[i] = input[i * 2] / 2 + input[(i * 2) + 1] / 2;
    }
    return input_size / 2;
}

int Utils::audio_upsample_2x(int16_t* input, int16_t* output, int input_size)
{
    for (int i = 0; i < input_size; i++)
    {
        output[2 * i] = input[i];
    }
    for (int i = 0; i < input_size - 1; i++)
    {
        output[2 * i + 1] =  output[2 * i] + (output[2 * i + 2] - output[2 * i]) / 2;
    }
    output[2 * input_size - 1] = input[input_size - 1];
    return input_size * 2;
}

void Utils::audio_adjust_gain(int16_t* input, int input_size, double gain) 
{
    for (int i = 0; i < input_size; i++) 
    {
        input[i] *= gain;
    }
}

} // LoraDv