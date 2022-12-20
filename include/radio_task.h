#ifndef RADIO_TASK_H
#define RADIO_TASK_H

#include <Arduino.h>
#include <memory>
#include <DebugLog.h>
#include <RadioLib.h>
#include <CircularBuffer.h>

#include "loradv_config.h"
#include "config.h"

namespace LoraDv {

class RadioTask {

public:
  RadioTask();

  void setup(const Config &config, TaskHandle_t taskHandle, uint32_t commandBits);

  void setFreq(long freq) const;

  bool hasData() const;
  bool readPacketSize(byte &packetSize);
  bool readNextByte(byte &b);

  void notifyTx();
  bool writePacketSize(byte packetSize);
  bool writeNextByte(byte b);

private:
  static const int CfgRadioQueueLen = 512;        // circular buffer length
  static const int CfgRadioPacketBufLen = 256;    // packet buffer length

  static const uint32_t CfgRadioRxBit = 0x01;     // task bit for rx
  static const uint32_t CfgRadioTxBit = 0x02;     // task bit for tx

  const int CfgRadioTaskStack = 4096;

private:
  void setupRig(long freq, long bw, int sf, int cr, int pwr, int sync, int crcBytes, bool isExplicit);

  static IRAM_ATTR void onRigIsrRxPacket();

  static void loraRadioTask(void *param);
  void loraRadioRxTx();
  void loraRadioRx(byte *packetBuf);
  void loraRadioTx(byte *packetBuf);

private:
  Config config_;

  std::shared_ptr<MODULE_NAME> rig_;

  static TaskHandle_t loraTaskHandle_;

  TaskHandle_t onRxPacketTaskHandle_;
  uint32_t onRxPacketCommandBits_;

  CircularBuffer<uint8_t, CfgRadioQueueLen> loraRadioRxQueue_;
  CircularBuffer<uint8_t, CfgRadioQueueLen> loraRadioRxQueueIndex_;
  CircularBuffer<uint8_t, CfgRadioQueueLen> loraRadioTxQueue_;
  CircularBuffer<uint8_t, CfgRadioQueueLen> loraRadioTxQueueIndex_;

  bool rigIsImplicitMode_;
  bool isIsrInstalled_;
  static volatile bool loraIsrEnabled_;
};

}

#endif // RADIO_TASK_H