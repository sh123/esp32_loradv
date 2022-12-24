#include "loradv_service.h"

namespace LoraDv {

std::shared_ptr<AiEsp32RotaryEncoder> Service::rotaryEncoder_;

Service::Service()
  : btnPressed_(false)
  , radioTask_(std::make_shared<RadioTask>())
  , audioTask_(std::make_shared<AudioTask>())
  , pmService_(std::make_shared<PmService>())
  , hwMonitor_(std::make_shared<HwMonitor>())
  , settingsMenu_(nullptr)
{
}

void Service::setup(std::shared_ptr<Config> config)
{
  config_ = config;

  LOG_SET_LEVEL(config_->LogLevel);
  LOG_SET_OPTION(false, false, true);  // disable file, line, enable func
  
  setupEncoder();
  setupScreen();

  LOG_INFO("PTT setup started");
  pinMode(config_->PttBtnPin_, INPUT);
  LOG_INFO("PTT setup completed");

  hwMonitor_->setup(config);
  pmService_->setup(config, display_);
  audioTask_->start(config, radioTask_, pmService_);
  radioTask_->start(config, audioTask_);

  updateScreen();

  LOG_INFO("Board setup completed");
}

void Service::setupEncoder() 
{
  LOG_INFO("Encoder setup started");
  rotaryEncoder_ = std::make_shared<AiEsp32RotaryEncoder>(config_->EncoderPinA_, config_->EncoderPinB_, 
    config_->EncoderPinBtn_, config_->EncoderPinVcc_, config_->EncoderSteps_);
  rotaryEncoder_->begin();
  rotaryEncoder_->setup(isrReadEncoder);
  LOG_INFO("Encoder setup completed");
}

void Service::setupScreen() 
{
  display_ = std::make_shared<Adafruit_SSD1306>(CfgDisplayWidth, CfgDisplayHeight, &Wire, -1);
  if(display_->begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    LOG_INFO("Display setup completed");
  } else {
    LOG_ERROR("Display init failed");
  }
}

IRAM_ATTR void Service::isrReadEncoder()
{
  rotaryEncoder_->readEncoder_ISR();
}

void Service::updateScreen() const
{
  display_->clearDisplay();
  display_->setTextSize(2);
  display_->setTextColor(WHITE);
  display_->setCursor(0, 0);
  display_->print(btnPressed_ ? "TX" : "RX"); display_->print(" "); 
  if (btnPressed_)
    display_->println((float)config_->LoraFreqTx / 1e6, 3);
  else
    display_->println((float)config_->LoraFreqRx / 1e6, 3);
  display_->print(audioTask_->getVolume()); display_->print("% "); 
  display_->print(hwMonitor_->getBatteryVoltage()); display_->print("V");
  display_->display();
}

bool Service::processPttButton()
{
  if (digitalRead(config_->PttBtnPin_) == LOW && !btnPressed_) {
    btnPressed_ = true;
    LOG_DEBUG("PTT pushed, start TX");
    audioTask_->setPtt(true);
    audioTask_->record();
    return true;
  } else if (digitalRead(config_->PttBtnPin_) == HIGH && btnPressed_) {
    btnPressed_ = false;
    LOG_DEBUG("PTT released");
    audioTask_->setPtt(false);
    return true;
  }
  return false;
}

bool Service::processRotaryEncoder()
{
  bool shouldUpdateScreen = false;
  long encoderDelta = rotaryEncoder_->encoderChanged();
  if (encoderDelta != 0)
  {
    LOG_INFO("Encoder changed:", rotaryEncoder_->readEncoder(), encoderDelta);
    if (settingsMenu_ == nullptr) {
      audioTask_->changeVolume(encoderDelta);
      shouldUpdateScreen = true;
    } else {
      settingsMenu_->onEncoderPositionChanged(encoderDelta);
      settingsMenu_->draw(display_);
    }
    pmService_->lightSleepReset();
  }
  if (rotaryEncoder_->isEncoderButtonClicked())
  {
    LOG_INFO("Encoder button clicked", esp_get_free_heap_size());
    if (settingsMenu_ == nullptr) {
      shouldUpdateScreen = true;
    } else {
      settingsMenu_->onEncoderButtonClicked();
      settingsMenu_->draw(display_);
    }
    pmService_->lightSleepReset();
  }
  if (rotaryEncoder_->isEncoderButtonClicked(CfgEncoderBtnLongMs))
  {
    LOG_INFO("Encoder button long clicked");
    if (settingsMenu_ == nullptr) {
      settingsMenu_ = std::make_shared<SettingsMenu>(config_);
      settingsMenu_->draw(display_);
    } else {
      settingsMenu_.reset();
      shouldUpdateScreen = true;
    }
    pmService_->lightSleepReset();
  }
  return shouldUpdateScreen;
}

void Service::loop() 
{
  if (processPttButton() || processRotaryEncoder() || pmService_->loop()) {
    updateScreen();
  }
}

} // LoraDv