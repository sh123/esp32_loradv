#ifndef AUDIO_CODEC_H
#define AUDIO_CODEC_H

#include <memory>
#include "loradv_config.h"

namespace LoraDv {

class AudioCodec {

public:
  virtual bool start(std::shared_ptr<const Config> config) = 0;
  virtual void stop() = 0;

  virtual int encode(uint8_t *encodedOut, int16_t *pcmIn) = 0;
  virtual int decode(int16_t *pcmOut, uint8_t *encodedIn, uint16_t encodedSize) = 0;

  virtual bool isFixedFrameSize() const = 0;
  
  virtual int getFrameSize() const = 0;
  virtual int getPcmFrameSize() const = 0;
  virtual int getPcmFrameBufferSize() const = 0;
};

} // namespace LoraDv

#endif // AUDIO_CODEC_H