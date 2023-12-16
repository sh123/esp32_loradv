#include "audio_codec_opus.h"

namespace LoraDv {

bool AudioCodecOpus::start(std::shared_ptr<const Config> config) 
{
  int encoderError;
  opusEncoder_ = opus_encoder_create(config->AudioSampleRate_, 1, OPUS_APPLICATION_VOIP, &encoderError);
  if (encoderError != OPUS_OK) {
    LOG_ERROR("Failed to create OPUS encoder, error", encoderError);
    return false;
  }
  encoderError = opus_encoder_init(opusEncoder_, config->AudioSampleRate_, 1, OPUS_APPLICATION_VOIP);
  if (encoderError != OPUS_OK) {
    LOG_ERROR("Failed to initialize OPUS encoder, error", encoderError);
    return false;
  }
  opus_encoder_ctl(opusEncoder_, OPUS_SET_BITRATE(config->AudioOpusRate));
  opus_encoder_ctl(opusEncoder_, OPUS_SET_COMPLEXITY(CfgComplexity));
  opus_encoder_ctl(opusEncoder_, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE));

  // configure decoder
  int decoderError;
  opusDecoder_ = opus_decoder_create(config->AudioSampleRate_, 1, &decoderError);
  if (decoderError != OPUS_OK) {
    LOG_ERROR("Failed to create OPUS decoder, error", decoderError);
    return false;
  } 

  pcmSampleSize_ = (int)(config->AudioSampleRate_ / 1000 * config->AudioOpusPcmLen);
  pcmSampleOutSize_ = 10 * pcmSampleSize_;
  encodedFrameSize_ = 1024;
  return true;
}

void AudioCodecOpus::stop() 
{
}

int AudioCodecOpus::encode(uint8_t *encodedOut, int16_t *pcmIn) 
{
  return opus_encode(opusEncoder_, pcmIn, pcmSampleSize_, encodedOut, encodedFrameSize_);
}

int AudioCodecOpus::decode(int16_t *pcmOut, uint8_t *encodedIn, uint16_t encodedSize) 
{
  return opus_decode(opusDecoder_, encodedIn, encodedSize, pcmOut, pcmSampleOutSize_, 0);
}

} // namespace LoraDv