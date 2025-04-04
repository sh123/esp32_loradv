#include "hal/radio_task.h"

namespace LoraDv {

volatile bool RadioTask::isIsrEnabled_ = true;
TaskHandle_t RadioTask::loraTaskHandle_;

RadioTask::RadioTask()
  : config_(nullptr)
  , rig_(nullptr)
  , audioTask_(nullptr)
  , cipher_(new ChaCha())
  , isRigImplicitMode_(false)
  , isIsrInstalled_(false)
  , isRunning_(false)
  , shouldUpdateScreen_(false)
  , lastRssi_(0)
{
}

void RadioTask::start(std::shared_ptr<const Config> config, std::shared_ptr<AudioTask> audioTask)
{
  config_ = config;
  audioTask_ = audioTask;
  cipher_->setKey(config->AudioPrivacyKey_, sizeof(config->AudioPrivacyKey_));
  xTaskCreatePinnedToCore(&task, "RadioTask", CfgRadioTaskStack, this, 2, &loraTaskHandle_, 1);
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
  LOG_INFO("Speed:", Utils::loraGetSpeed(sf, cr, bw), "bps");
  LOG_INFO("Min level:", Utils::loraGetSnrLimit(sf, bw));
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
    rig_->setDio0Action(onRigIsrRxPacket, RISING);
    isIsrInstalled_ = true;
#endif
  rig_->explicitHeader();
  LOG_INFO("LoRa initialized");
}

void RadioTask::setupRigFsk(long freq, float bitRate, float freqDev, float rxBw, int pwr, byte shaping)
{
  LOG_INFO("Initializing FSK");
  LOG_INFO("Frequency:", freq, "Hz");
  LOG_INFO("Bit rate:", bitRate, "kbps");
  LOG_INFO("Deviation:", freqDev, "kHz");
  LOG_INFO("Bandwidth:", rxBw, "kHz");
  LOG_INFO("Power:", pwr, "dBm");
  LOG_INFO("Shaping:", shaping);
  rig_ = std::make_shared<MODULE_NAME>(new Module(config_->LoraPinSs_, config_->LoraPinA_, config_->LoraPinRst_, config_->LoraPinB_));
  int state = rig_->beginFSK((float)freq / 1e6, bitRate, freqDev, rxBw, pwr);
  if (state != RADIOLIB_ERR_NONE) {
    LOG_ERROR("Radio start error:", state);
  }
  rig_->disableAddressFiltering();
  rig_->setDataShaping(shaping);
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
    rig_->setDio0Action(onRigIsrRxPacket, RISING);
    isIsrInstalled_ = true;
#endif
  LOG_INFO("FSK initialized");
}

void RadioTask::setFreq(long loraFreq) const 
{
  rig_->setFrequency((float)loraFreq / (float)1e6);
}

bool RadioTask::hasData() const 
{
  return loraRadioRxQueue_.index.size() > 0;
}

bool RadioTask::readPacketSize(byte &packetSize)
{
  if (!hasData()) return false;
  packetSize = loraRadioRxQueue_.index.shift();
  return true;
}

bool RadioTask::readNextByte(byte &b)
{
  if (loraRadioRxQueue_.data.size() == 0) return false;
  b = loraRadioRxQueue_.data.shift();
  return true;
}

bool RadioTask::writePacketSize(byte packetSize)
{
  return loraRadioTxQueue_.index.push(packetSize);
}

bool RadioTask::writeNextByte(byte b) 
{
  return loraRadioTxQueue_.data.push(b);
}

IRAM_ATTR void RadioTask::onRigIsrRxPacket() 
{
  if (!isIsrEnabled_) return;
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
  LOG_INFO("Radio task started");
  isRunning_ = true;

  if (config_->ModType == CFG_MOD_TYPE_LORA) {
    setupRig(config_->LoraFreqRx, config_->LoraBw, config_->LoraSf, 
      config_->LoraCodingRate, config_->LoraPower, config_->LoraSync_, config_->LoraCrc_);
  } else {
    setupRigFsk(config_->LoraFreqRx, config_->FskBitRate, config_->FskFreqDev,
      config_->FskRxBw, config_->LoraPower, config_->FskShaping);
  }
  randomSeed(rig_->random(0x7FFFFFFF));
  rigTaskStartReceive();

  byte *packetBuf = new byte[CfgRadioPacketBufLen];
  byte *tmpBuf = new byte[CfgRadioPacketBufLen];

  while (isRunning_) {
    uint32_t cmdBits = 0;
    xTaskNotifyWaitIndexed(0, 0x00, ULONG_MAX, &cmdBits, portMAX_DELAY);

    LOG_DEBUG("Radio task bits", cmdBits);
    if (cmdBits & CfgRadioRxBit) {
      rigTaskReceive(packetBuf, tmpBuf);
    }
    else if (cmdBits & CfgRadioTxBit) {
      rigTaskTransmit(packetBuf, tmpBuf);
    } 
    if (cmdBits & CfgRadioRxStartBit) {
      rigTaskStartReceive();
    }
    else if (cmdBits & CfgRadioTxStartBit) {
      rigTaskStartTransmit();
    }
  } 

  delete tmpBuf;
  delete packetBuf;
  LOG_INFO("Radio task stopped");
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
  isIsrEnabled_ = true;
}

void RadioTask::rigTaskStartTransmit() 
{
  LOG_INFO("Start transmit");
  isIsrEnabled_ = false;
  if (isHalfDuplex()) setFreq(config_->LoraFreqTx);
}

void RadioTask::rigTaskReceive(byte *packetBuf, byte *tmpBuf) 
{
  int packetSize = rig_->getPacketLength();
  if (packetSize > 8 && packetSize < CfgRadioPacketBufLen) {
    // receive packet
    int state = rig_->readData(packetBuf, packetSize);
    if (state == RADIOLIB_ERR_NONE) {
      byte *receiveBuf = packetBuf;
      // if privacy enabled
      if (config_->AudioEnPriv){
        // read iv and decrypt packet
        cipher_->setIV(packetBuf, sizeof(iv_));
        packetSize -= sizeof(iv_);
        cipher_->decrypt(tmpBuf, packetBuf + sizeof(iv_), packetSize);
        receiveBuf = tmpBuf;
      }
      // send packet to the queue
      LOG_DEBUG("Received packet, size", packetSize);
      for (int i = 0; i < packetSize; i++) {
        loraRadioRxQueue_.data.push(receiveBuf[i]);
      }
      loraRadioRxQueue_.index.push(packetSize);
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
  } else {
    LOG_ERROR("Wrong packet size: ", packetSize);
  }
}

void RadioTask::rigTaskTransmit(byte *packetBuf, byte *tmpBuf) 
{
  while (loraRadioTxQueue_.index.size() > 0) {
    // fetch packet size and packet from the queue
    int txBytesCnt = loraRadioTxQueue_.index.shift();
    for (int i = 0; i < txBytesCnt; i++) {
        packetBuf[i] = loraRadioTxQueue_.data.shift();
    }
    byte *sendBuf = packetBuf;
    // if privacy enabled
    if (config_->AudioEnPriv) {
      // generate IV
      for (int i = 0; i < sizeof(iv_); i++) {
        iv_[i] = random(255);
        tmpBuf[i] = iv_[i];
      }
      // encrypt
      cipher_->setIV(iv_, sizeof(iv_));
      cipher_->encrypt(tmpBuf + sizeof(iv_), packetBuf, txBytesCnt);
      txBytesCnt += sizeof(iv_);
      sendBuf = tmpBuf;
    }
    // transmit
    int loraRadioState = rig_->transmit(sendBuf, txBytesCnt);
    if (loraRadioState != RADIOLIB_ERR_NONE) {
        LOG_ERROR("Radio transmit failed:", loraRadioState, txBytesCnt);
    } else {
      LOG_DEBUG("Transmitted packet", txBytesCnt);
    }
    vTaskDelay(1);
  }
}

} // LoraDv