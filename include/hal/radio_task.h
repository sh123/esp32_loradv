#ifndef RADIO_TASK_H
#define RADIO_TASK_H

#include <Arduino.h>
#include <memory>
#include <DebugLog.h>
#include <RadioLib.h>
#include <CircularBuffer.hpp>
#include <ChaCha.h>

#include "settings/config.h"
#include "audio/audio_task.h"
#include "utils/utils.h"
#include "settings/settings_menu.h"

namespace LoraDv {

class AudioTask;

class RadioTask {

public:
  explicit RadioTask(std::shared_ptr<const Config> config);

  void start(std::shared_ptr<AudioTask> audioTask);
  inline void stop() { isRunning_ = false; }
  bool loop();

  void setFreq(long freq) const;
  inline bool isHalfDuplex() const { return config_->LoraFreqTx != config_->LoraFreqRx; }
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
  constexpr static int CfgCoreId = 1;                   // core id where task will run
  constexpr static int CfgTaskPriority = 2;             // task priority

  static constexpr int CfgRadioQueueLen = 512;          // circular buffer length
  static constexpr int CfgRadioPacketBufLen = 256;      // packet buffer length

  static constexpr uint32_t CfgRadioRxBit = 0x01;       // task bit for rx
  static constexpr uint32_t CfgRadioTxBit = 0x02;       // task bit for tx
  static constexpr uint32_t CfgRadioRxStartBit = 0x04;  // task bit for start rx
  static constexpr uint32_t CfgRadioTxStartBit = 0x10;  // task bit for start tx

  static constexpr int CfgRadioTaskStack = 4096;        // task stack size
  static constexpr size_t CfgIvSize = 8;                // IV, initialization vector size

private:
  void setupRig(long freq, long bw, int sf, int cr, int pwr, int sync, int crcBytes);
  void setupRigFsk(long freq, float bitRate, float freqDev, float rxBw, int pwr, byte shaping);

  static IRAM_ATTR void onRigIsrRxPacket();

  static void task(void *param);

  void rigTask();
  void rigTaskReceive(byte *packetBuf, byte *tmpBuf);
  void rigTaskTransmit(byte *packetBuf, byte *tmpBuf);
  void rigTaskStartReceive();
  void rigTaskStartTransmit();

private:
  std::shared_ptr<const Config> config_;

  std::shared_ptr<MODULE_NAME> radioModule_;
  std::shared_ptr<AudioTask> audioTask_;

  uint8_t iv_[CfgIvSize];
  std::shared_ptr<ChaCha> cipher_;

  static TaskHandle_t loraTaskHandle_;

  struct RadioQueue {
    CircularBuffer<uint8_t, CfgRadioQueueLen> data;
    CircularBuffer<uint8_t, CfgRadioQueueLen> index;
  };

  RadioQueue radioRxQueue_;
  RadioQueue radioTxQueue_;

  bool isImplicitMode_;
  bool isIsrInstalled_;
  static volatile bool isIsrEnabled_;
  volatile bool isRunning_;
  volatile bool shouldUpdateScreen_;
  float lastRssi_;
};

}

#endif // RADIO_TASK_H