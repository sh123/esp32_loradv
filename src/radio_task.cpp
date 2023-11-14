#include "radio_task.h"

namespace LoraDv {

volatile bool RadioTask::loraIsrEnabled_ = true;
TaskHandle_t RadioTask::loraTaskHandle_;

RadioTask::RadioTask()
  : rigIsImplicitMode_(false)
  , isIsrInstalled_(false)
  , isRunning_(false)
  , shouldUpdateScreen_(false)
  , lastRssi_(0)
{
}

void RadioTask::start(std::shared_ptr<Config> config, std::shared_ptr<AudioTask> audioTask)
{
  config_ = config;
  audioTask_ = audioTask;
  xTaskCreate(&task, "RadioTask", CfgRadioTaskStack, this, 5, &loraTaskHandle_);
}

float RadioTask::getSnrLimit(int sf, long bw) 
{
  float snrLimit = -7;
  switch (sf) {
    case 7:
        snrLimit = -7.5;
        break;
    case 8:
        snrLimit = -10.0;
        break;
    case 9:
        snrLimit = -12.6;
        break;
    case 10:
        snrLimit = -15.0;
        break;
    case 11:
        snrLimit = -17.5;
        break;
    case 12:
        snrLimit = -20.0;
        break;
  }
  return -174 + 10 * log10(bw) + 6 + snrLimit;
}

void RadioTask::setupRig(long loraFreq, long bw, int sf, int cr, int pwr, int sync, int crcBytes)
{
  LOG_INFO("Initializing LoRa");
  LOG_INFO("Frequency:", loraFreq, "Hz");
  LOG_INFO("Bandwidth:", bw, "Hz");
  LOG_INFO("Spreading:", sf);
  LOG_INFO("Coding rate:", cr);
  LOG_INFO("Power:", pwr, "dBm");
  LOG_INFO("Sync:", "0x" + String(sync, HEX));
  LOG_INFO("CRC:", crcBytes);
  LOG_INFO("Speed:", getSpeed(sf, cr, bw), "bps");
  LOG_INFO("Min level:", getSnrLimit(sf, bw));
  rig_ = std::make_shared<MODULE_NAME>(new Module(config_->LoraPinSs_, config_->LoraPinA_, config_->LoraPinRst_, config_->LoraPinB_));
  int state = rig_->begin((float)loraFreq / 1e6, (float)bw / 1e3, sf, cr, sync, pwr);
  if (state != RADIOLIB_ERR_NONE) {
    LOG_ERROR("Radio start error:", state);
  }
  rig_->setCRC(crcBytes);
  rig_->setPreambleLength(config_->LoraPreambleLen_);
#ifdef USE_SX126X
    #pragma message("Using SX126X")
    LOG_INFO("Using SX126X module");
    rig_->setRfSwitchPins(config_->LoraPinSwitchRx_, config_->LoraPinSwitchTx_);
    if (isIsrInstalled_) rig_->clearDio1Action();
    rig_->setDio1Action(onRigIsrRxPacket);
    isIsrInstalled_ = true;
#else
    #pragma message("Using SX127X")
    LOG_INFO("Using SX127X module");
    if (isIsrInstalled_) rig_->clearDio0Action();
    rig_->setDio0Action(onRigIsrRxPacket);
    isIsrInstalled_ = true;
#endif
  rig_->explicitHeader();
  
  state = rig_->startReceive();
  if (state != RADIOLIB_ERR_NONE) {
    LOG_ERROR("Receive start error:", state);
  }

  LOG_INFO("LoRa initialized");
}

void RadioTask::setFreq(long loraFreq) const 
{
  rig_->setFrequency((float)loraFreq / (float)1e6);
}

bool RadioTask::hasData() const 
{
  return loraRadioRxQueueIndex_.size() > 0;
}

bool RadioTask::readPacketSize(byte &packetSize)
{
  if (!hasData()) return false;
  packetSize = loraRadioRxQueueIndex_.shift();
  return true;
}

bool RadioTask::readNextByte(byte &b)
{
  if (loraRadioRxQueue_.size() == 0) return false;
  b = loraRadioRxQueue_.shift();
  return true;
}

bool RadioTask::writePacketSize(byte packetSize)
{
  return loraRadioTxQueueIndex_.push(packetSize);
}

bool RadioTask::writeNextByte(byte b) 
{
  return loraRadioTxQueue_.push(b);
}

IRAM_ATTR void RadioTask::onRigIsrRxPacket() 
{
  if (!loraIsrEnabled_) return;
  BaseType_t xHigherPriorityTaskWoken;
  xTaskNotifyFromISR(loraTaskHandle_, CfgRadioRxBit, eSetBits, &xHigherPriorityTaskWoken);
}

void RadioTask::task(void *param)
{
  reinterpret_cast<RadioTask*>(param)->rigTask();
}

void RadioTask::startTransmit() const
{
  xTaskNotify(loraTaskHandle_, CfgRadioTxStartBit, eSetBits);
}

void RadioTask::startReceive() const
{
  xTaskNotify(loraTaskHandle_, CfgRadioRxStartBit, eSetBits);
}

void RadioTask::transmit() const
{
  xTaskNotify(loraTaskHandle_, CfgRadioTxBit, eSetBits);
}

void RadioTask::rigTask() 
{
  LOG_INFO("Lora task started");
  isRunning_ = true;

  setupRig(config_->LoraFreqRx, config_->LoraBw, config_->LoraSf, 
    config_->LoraCodingRate, config_->LoraPower, config_->LoraSync_, config_->LoraCrc_);

  byte *packetBuf = new byte[CfgRadioPacketBufLen];

  while (isRunning_) {
    uint32_t cmdBits = 0;
    xTaskNotifyWaitIndexed(0, 0x00, ULONG_MAX, &cmdBits, portMAX_DELAY);

    LOG_DEBUG("Lora task bits", cmdBits);
    if (cmdBits & CfgRadioRxBit) {
      rigTaskReceive(packetBuf);
    }
    else if (cmdBits & CfgRadioTxBit) {
      rigTaskTransmit(packetBuf);
    } 
    if (cmdBits & CfgRadioRxStartBit) {
      rigTaskStartReceive();
    }
    else if (cmdBits & CfgRadioTxStartBit) {
      rigTaskStartTransmit();
    }
  } 

  delete packetBuf;
  LOG_INFO("Lora task stopped");
  vTaskDelete(NULL);
}

bool RadioTask::loop() 
{
  bool shouldUpdateScreen = shouldUpdateScreen_;
  shouldUpdateScreen_ = false;
  return shouldUpdateScreen;
}

void RadioTask::rigTaskStartReceive() 
{
  LOG_INFO("Start receive");
  if (isHalfDuplex()) setFreq(config_->LoraFreqRx);
  int loraRadioState = rig_->startReceive();
  if (loraRadioState != RADIOLIB_ERR_NONE) {
    LOG_ERROR("Start receive error: ", loraRadioState);
  }
  vTaskDelay(1);
  loraIsrEnabled_ = true;
}

void RadioTask::rigTaskStartTransmit() 
{
  LOG_INFO("Start transmit");
  loraIsrEnabled_ = false;
  if (isHalfDuplex()) setFreq(config_->LoraFreqTx);
}

void RadioTask::rigTaskReceive(byte *packetBuf) 
{
  int packetSize = rig_->getPacketLength();
  if (packetSize > 0 && packetSize < CfgRadioPacketBufLen) {
    int state = rig_->readData(packetBuf, packetSize);
    if (state == RADIOLIB_ERR_NONE) {
      // process packet
      LOG_DEBUG("Received packet, size", packetSize);
      for (int i = 0; i < packetSize; i++) {
        loraRadioRxQueue_.push(packetBuf[i]);
      }
      loraRadioRxQueueIndex_.push(packetSize);
      audioTask_->play();
    } else {
      LOG_ERROR("Read data error: ", state);
    }
    lastRssi_ = rig_->getRSSI();
    // probably not needed, still in receive
    state = rig_->startReceive();
    if (state != RADIOLIB_ERR_NONE) {
      LOG_ERROR("Start receive error: ", state);
    }
  }
}

void RadioTask::rigTaskTransmit(byte *packetBuf) 
{
  while (loraRadioTxQueueIndex_.size() > 0) {
    int txBytesCnt = loraRadioTxQueueIndex_.shift();
    for (int i = 0; i < txBytesCnt; i++) {
        packetBuf[i] = loraRadioTxQueue_.shift();
    }
    int loraRadioState = rig_->transmit(packetBuf, txBytesCnt);
    if (loraRadioState != RADIOLIB_ERR_NONE) {
        LOG_ERROR("Lora radio transmit failed:", loraRadioState);
    }
    LOG_DEBUG("Transmitted packet", txBytesCnt);
    vTaskDelay(1);
  }
}

} // LoraDv