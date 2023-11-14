#ifndef RADIO_TASK_H
#define RADIO_TASK_H

#include <Arduino.h>
#include <memory>
#include <DebugLog.h>
#include <RadioLib.h>
#include <CircularBuffer.h>

#include "loradv_config.h"
#include "audio_task.h"
#include "config.h"

namespace LoraDv {

class AudioTask;

class RadioTask {

public:
  RadioTask();

  void start(std::shared_ptr<Config> config, std::shared_ptr<AudioTask> audioTask);
  inline void stop() { isRunning_ = false; }
  bool loop();

  void setFreq(long freq) const;
  inline bool isHalfDuplex() const { return config_->LoraFreqTx != config_->LoraFreqRx; }

  static int getSpeed(int sf, int cr, long bw) { return (int)(sf * (4.0 / cr) / (pow(2.0, sf) / bw)); }
  static float getSnrLimit(int sf, long bw);
  inline float getRssi() const { return lastRssi_; }

  bool hasData() const;
  bool readPacketSize(byte &packetSize);
  bool readNextByte(byte &b);

  void transmit() const;
  void startTransmit() const;
  void startReceive() const;
  
  bool writePacketSize(byte packetSize);
  bool writeNextByte(byte b);

private:
  static const int CfgRadioQueueLen = 512;          // circular buffer length
  static const int CfgRadioPacketBufLen = 256;      // packet buffer length

  static const uint32_t CfgRadioRxBit = 0x01;       // task bit for rx
  static const uint32_t CfgRadioTxBit = 0x02;       // task bit for tx
  static const uint32_t CfgRadioRxStartBit = 0x04;  // task bit for start rx
  static const uint32_t CfgRadioTxStartBit = 0x10;  // task bit for start tx

  const int CfgRadioTaskStack = 4096;

private:
  void setupRig(long freq, long bw, int sf, int cr, int pwr, int sync, int crcBytes, bool isExplicit);

  static IRAM_ATTR void onRigIsrRxPacket();

  static void task(void *param);

  void rigTask();
  void rigTaskReceive(byte *packetBuf);
  void rigTaskTransmit(byte *packetBuf);
  void rigTaskStartReceive();
  void rigTaskStartTransmit();

private:
  std::shared_ptr<Config> config_;

  std::shared_ptr<MODULE_NAME> rig_;
  std::shared_ptr<AudioTask> audioTask_;

  static TaskHandle_t loraTaskHandle_;

  CircularBuffer<uint8_t, CfgRadioQueueLen> loraRadioRxQueue_;
  CircularBuffer<uint8_t, CfgRadioQueueLen> loraRadioRxQueueIndex_;
  CircularBuffer<uint8_t, CfgRadioQueueLen> loraRadioTxQueue_;
  CircularBuffer<uint8_t, CfgRadioQueueLen> loraRadioTxQueueIndex_;

  bool rigIsImplicitMode_;
  bool isIsrInstalled_;
  static volatile bool loraIsrEnabled_;
  volatile bool isRunning_;
  volatile bool shouldUpdateScreen_;
  float lastRssi_;
};

}

#endif // RADIO_TASK_H