#ifndef AUDIO_CODEC2_H
#define AUDIO_CODEC2_H

#include "audio_codec.h"

namespace LoraDv {

class AudioCodecCodec2 : AudioCodec {

public:
  virtual bool start(std::shared_ptr<const Config> config) override;
  virtual void stop() override;

  virtual int encode(uint8_t *encodedOut, int16_t *pcmIn) override;
  virtual int decode(int16_t *pcmOut, uint8_t *encodedIn, uint16_t encodedSize) override;

  virtual bool isFixedFrameSize() const override { return true; }

  virtual int getBytesPerFrame() const override;
  virtual int getPcmSamplesPerFrame() const override;

private:
  struct CODEC2 *codec_; 

  int codecSamplesPerFrame_;
  int codecBytesPerFrame_;
};

} // namespace LoraDv

#endif // AUDIO_CODEC2_H
