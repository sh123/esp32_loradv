#include "radio_task.h"

namespace LoraDv {

volatile bool RadioTask::loraIsrEnabled_ = true;
TaskHandle_t RadioTask::loraTaskHandle_;

RadioTask::RadioTask()
  : rigIsImplicitMode_(false)
  , isIsrInstalled_(false)
{
}

void RadioTask::setup(const Config &config, std::shared_ptr<AudioTask> audioTask)
{
  config_ = config;
  audioTask_ = audioTask;
  xTaskCreate(&loraRadioTask, "lora_task", CfgRadioTaskStack, this, 5, &loraTaskHandle_);
}

void RadioTask::setupRig(long loraFreq, long bw, int sf, int cr, int pwr, int sync, int crcBytes, bool isExplicit)
{
  rigIsImplicitMode_ = !isExplicit;
  rigIsImplicitMode_ = sf == 6;      // must be implicit for SF6
  int loraSpeed = (int)(sf * (4.0 / cr) / (pow(2.0, sf) / bw));

  LOG_INFO("Initializing LoRa");
  LOG_INFO("Frequency:", loraFreq, "Hz");
  LOG_INFO("Bandwidth:", bw, "Hz");
  LOG_INFO("Spreading:", sf);
  LOG_INFO("Coding rate:", cr);
  LOG_INFO("Power:", pwr, "dBm");
  LOG_INFO("Sync:", "0x" + String(sync, HEX));
  LOG_INFO("CRC:", crcBytes);
  LOG_INFO("Header:", rigIsImplicitMode_ ? "implicit" : "explicit");
  LOG_INFO("Speed:", loraSpeed, "bps");
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
  LOG_INFO("Min level:", -174 + 10 * log10(bw) + 6 + snrLimit, "dBm");
  rig_ = std::make_shared<MODULE_NAME>(new Module(config_.LoraPinSs, config_.LoraPinA, config_.LoraPinRst, config_.LoraPinB));
  int state = rig_->begin((float)loraFreq / 1e6, (float)bw / 1e3, sf, cr, sync, pwr);
  if (state != RADIOLIB_ERR_NONE) {
    LOG_ERROR("Radio start error:", state);
  }
  rig_->setCRC(crcBytes);
#ifdef USE_SX126X
    #pragma message("Using SX126X")
    LOG_INFO("Using SX126X module");
    rig_->setRfSwitchPins(config_.LoraPinSwitchRx, config_.LoraPinSwitchTx);
    if (isIsrInstalled_) rig_->clearDio1Action();
    rig_->setDio1Action(onRigIsrRxPacket);
    isIsrInstalled_ = true;
#else
    #pragma message("Using SX127X")
    LOG_INFO("Using SX127X module");
    if (isIsrInstalled_) radio_->clearDio0Action();
    radio_->setDio0Action(onRigIsrRxPacket);
    isIsrInstalled_ = true;
#endif

  if (rigIsImplicitMode_) {
    rig_->implicitHeader(0xff);
  } else {
    rig_->explicitHeader();
  }
  
  state = rig_->startReceive();
  if (state != RADIOLIB_ERR_NONE) {
    LOG_ERROR("Receive start error:", state);
  }

  LOG_INFO("LoRa initialized");
}

void RadioTask::setFreq(long loraFreq) const 
{
  rig_->setFrequency((float)loraFreq / (float)1e6);
  int state = rig_->startReceive();
  if (state != RADIOLIB_ERR_NONE) {
    LOG_ERROR("Start receive error:", state);
  }
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

void RadioTask::loraRadioTask(void *param)
{
  reinterpret_cast<RadioTask*>(param)->loraRadioRxTx();
}

void RadioTask::notifyTx() 
{
  xTaskNotify(loraTaskHandle_, CfgRadioTxBit, eSetBits);
}

void RadioTask::loraRadioRxTx() 
{
  LOG_INFO("Lora task started");

  setupRig(config_.LoraFreqRx, config_.LoraBw, config_.LoraSf, 
    config_.LoraCodingRate, config_.LoraPower, config_.LoraSync, config_.LoraCrc, config_.LoraExplicit);

  byte *packetBuf = new byte[CfgRadioPacketBufLen];

  while (true) {
    uint32_t cmdBits = 0;
    xTaskNotifyWaitIndexed(0, 0x00, ULONG_MAX, &cmdBits, portMAX_DELAY);

    LOG_DEBUG("Lora task bits", cmdBits);
    if (cmdBits & CfgRadioRxBit) {
      loraRadioRx(packetBuf);
    }
    else if (cmdBits & CfgRadioTxBit) {
      loraRadioTx(packetBuf);
    }
  } 
}

void RadioTask::loraRadioRx(byte *packetBuf) 
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
      audioTask_->notifyPlay();
    } else {
      LOG_ERROR("Read data error: ", state);
    }
    // probably not needed, still in receive
    state = rig_->startReceive();
    if (state != RADIOLIB_ERR_NONE) {
      LOG_ERROR("Start receive error: ", state);
    }
    // TODO, reset light sleep
  }
}

void RadioTask::loraRadioTx(byte *packetBuf) 
{
  loraIsrEnabled_ = false;
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
    // TODO, reset light sleep
  }
  int loraRadioState = rig_->startReceive();
  if (loraRadioState != RADIOLIB_ERR_NONE) {
    LOG_ERROR("Start receive error: ", loraRadioState);
  }
  loraIsrEnabled_ = true;
}

} // LoraDv