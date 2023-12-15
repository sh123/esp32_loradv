#include "audio_codec2.h"

namespace LoraDv {

bool AudioCodec2::start(std::shared_ptr<const Config> config) 
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

void AudioCodec2::stop() 
{
  codec2_destroy(codec_);
}

int AudioCodec2::encode(uint8_t *encodedOut, int16_t *pcmIn) 
{
    codec2_encode(codec_, encodedOut, pcmIn);
    return codecBytesPerFrame_;
}

int AudioCodec2::decode(int16_t *pcmOut, uint8_t *encodedIn, uint16_t encodedSize)
{
    codec2_decode(codec_, pcmOut, encodedIn);
    return codecSamplesPerFrame_;
}

} // namespace LoraDv