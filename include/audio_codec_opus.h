#ifndef AUDIO_CODEC_OPUS_H
#define AUDIO_CODEC_OPUS_H

#include <opus.h>
#include "audio_codec.h"

namespace LoraDv {

class AudioCodecOpus : AudioCodec {

public:
  virtual bool start(std::shared_ptr<const Config> config) override;
  virtual void stop() override;

  virtual int encode(uint8_t *encodedOut, int16_t *pcmIn) override;
  virtual int decode(int16_t *pcmOut, uint8_t *encodedIn, uint16_t encodedSize) override;

private:
  const int CfgComplexity = 0;

  OpusEncoder *opusEncoder_;
  OpusDecoder *opusDecoder_;

  int pcmSampleSize_;
  int pcmSampleOutSize_;
  int encodedFrameSize_;
};

}

#endif // AUDIO_CODEC_OPUS_H