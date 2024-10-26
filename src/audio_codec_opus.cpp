#include "audio_codec_opus.h"

namespace LoraDv {

AudioCodecOpus::AudioCodecOpus() 
  : opusEncoder_(0)
  , opusDecoder_(0)
  , pcmFrameSize_(0)
  , pcmFrameBufferSize_(0)
  , encodedFrameBufferSize_(0)
{
}

bool AudioCodecOpus::start(std::shared_ptr<const Config> config) 
{
  int encoderError;
  opusEncoder_ = opus_encoder_create(config->AudioCodecSampleRate_, 1, OPUS_APPLICATION_VOIP, &encoderError);
  if (encoderError != OPUS_OK) {
    LOG_ERROR("Failed to create OPUS encoder, error", encoderError);
    return false;
  }
  encoderError = opus_encoder_init(opusEncoder_, config->AudioCodecSampleRate_, 1, OPUS_APPLICATION_VOIP);
  if (encoderError != OPUS_OK) {
    LOG_ERROR("Failed to initialize OPUS encoder, error", encoderError);
    return false;
  }
  opus_encoder_ctl(opusEncoder_, OPUS_SET_BITRATE(config->AudioOpusRate));
  opus_encoder_ctl(opusEncoder_, OPUS_SET_COMPLEXITY(CfgComplexity));
  opus_encoder_ctl(opusEncoder_, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE));

  // configure decoder
  int decoderError;
  opusDecoder_ = opus_decoder_create(config->AudioCodecSampleRate_, 1, &decoderError);
  if (decoderError != OPUS_OK) {
    LOG_ERROR("Failed to create OPUS decoder, error", decoderError);
    return false;
  } 

  pcmFrameSize_ = (int)(config->AudioCodecSampleRate_ / 1000 * config->AudioOpusPcmLen);
  pcmFrameBufferSize_ = 10 * pcmFrameSize_;
  encodedFrameBufferSize_ = CfgEncodedFrameBufferSize;
  return true;
}

void AudioCodecOpus::stop() 
{
  opus_encoder_destroy(opusEncoder_);
  opus_decoder_destroy(opusDecoder_);
}

int AudioCodecOpus::encode(uint8_t *encodedOut, int16_t *pcmIn) 
{
  return opus_encode(opusEncoder_, pcmIn, pcmFrameSize_, encodedOut, encodedFrameBufferSize_);
}

int AudioCodecOpus::decode(int16_t *pcmOut, uint8_t *encodedIn, uint16_t encodedSize) 
{
  return opus_decode(opusDecoder_, encodedIn, encodedSize, pcmOut, pcmFrameBufferSize_, 0);
}

} // namespace LoraDv