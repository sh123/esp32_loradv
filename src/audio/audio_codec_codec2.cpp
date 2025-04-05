#include "audio/audio_codec_codec2.h"

namespace LoraDv {

AudioCodecCodec2::AudioCodecCodec2()
  : codec_(0)
  , codecSamplesPerFrame_(0)
  , codecBytesPerFrame_(0)
{
}

bool AudioCodecCodec2::start(std::shared_ptr<const Config> config) 
{
  codec_ = codec2_create(config->AudioCodec2Mode);
  if (codec_ == NULL) {
    LOG_ERROR("Failed to create Codec2");
    return false;
  }
  codecSamplesPerFrame_ = codec2_samples_per_frame(codec_);
  codecBytesPerFrame_ = codec2_bytes_per_frame(codec_);
  LOG_INFO("Codec2 started", config->AudioCodec2Mode, codecSamplesPerFrame_, codecBytesPerFrame_);
  return true;
}

void AudioCodecCodec2::stop() 
{
  codec2_destroy(codec_);
}

int AudioCodecCodec2::encode(uint8_t *encodedOut, int16_t *pcmIn) 
{
    codec2_encode(codec_, encodedOut, pcmIn);
    return codecBytesPerFrame_;
}

int AudioCodecCodec2::decode(int16_t *pcmOut, uint8_t *encodedIn, uint16_t encodedSize)
{
    codec2_decode(codec_, pcmOut, encodedIn);
    return codecSamplesPerFrame_;
}

int AudioCodecCodec2::getFrameSize() const
{
  return codec2_bytes_per_frame(codec_);
}

int AudioCodecCodec2::getPcmFrameSize() const
{
  return codec2_samples_per_frame(codec_);
}

int AudioCodecCodec2::getPcmFrameBufferSize() const
{
  return codec2_samples_per_frame(codec_);
}

} // namespace LoraDv