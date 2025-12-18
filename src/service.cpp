#include "service.h"

namespace LoraDv {

std::shared_ptr<AiEsp32RotaryEncoder> Service::rotaryEncoder_;

Service::Service(std::shared_ptr<Config> config)
  : config_(config)
  , display_(std::make_shared<Adafruit_SSD1306>(CfgDisplayWidth, CfgDisplayHeight, &Wire, -1))
  , pmService_(std::make_shared<PmService>(config, display_))
  , hwMonitor_(std::make_shared<HwMonitor>(config))
  , radioTask_(std::make_shared<RadioTask>(config))
  , audioTask_(std::make_shared<AudioTask>(config, pmService_))
  , settingsMenu_(nullptr)
  , btnPressed_(false)
{
  rotaryEncoder_ = std::make_shared<AiEsp32RotaryEncoder>(config->EncoderPinA_, config->EncoderPinB_, 
    config->EncoderPinBtn_, config->EncoderPinVcc_, config->EncoderSteps_);
}

void Service::setup()
{
  LOG_SET_LEVEL(config_->LogLevel);
  LOG_INFO("Board setup started");
 
  // setup bootloader random source as WiFi and BT are not used
  bootloader_random_enable();

  setupEncoder();
  setupScreen();
  setupPttButton();

  audioTask_->start(radioTask_);
  radioTask_->start(audioTask_);

  updateScreen();

  LOG_INFO("Board setup completed");
}

void Service::setupEncoder() 
{
  LOG_INFO("Encoder setup started");
  rotaryEncoder_->begin();
  rotaryEncoder_->setup(isrReadEncoder);
  LOG_INFO("Encoder setup completed");
}

void Service::setupScreen() 
{
  if(display_->begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    LOG_INFO("Display setup completed");
  } else {
    LOG_ERROR("Display init failed");
  }
}

void Service::setupPttButton() 
{
  LOG_INFO("PTT setup started");
  pinMode(config_->PttBtnPin_, INPUT);
  LOG_INFO("PTT setup completed");
}

IRAM_ATTR void Service::isrReadEncoder()
{
  rotaryEncoder_->readEncoder_ISR();
}

void Service::updateScreen() const
{
  bool isPlaying = audioTask_->isPlaying();
  display_->clearDisplay();
  display_->setTextColor(WHITE);
  display_->setCursor(0, 0);

  display_->setTextSize(1);
  display_->print("["); display_->print(audioTask_->getVolume()); display_->print("] "); 
  display_->print(hwMonitor_->getBatteryVoltage()); display_->print("V ");
  if (isPlaying)
    display_->print(radioTask_->getRssi());
  display_->println();

  display_->setCursor(0, CfgDisplayHeight/2 + 2);
  display_->setTextSize(2);
  if (btnPressed_)
    display_->print((float)config_->LoraFreqTx / 1e6, 3);
  else
    display_->print((float)config_->LoraFreqRx / 1e6, 3);
  display_->print(" "); 
  display_->print(btnPressed_ ? "TX" : isPlaying ? "RX" : "--");
  display_->println();

  display_->display();
}

bool Service::processPttButton()
{
  if (digitalRead(config_->PttBtnPin_) == LOW && !btnPressed_) {
    btnPressed_ = true;
    LOG_INFO("PTT pushed, start TX");
    audioTask_->setPtt(true);
    audioTask_->record();
    return true;
  } else if (digitalRead(config_->PttBtnPin_) == HIGH && btnPressed_) {
    btnPressed_ = false;
    LOG_INFO("PTT released");
    audioTask_->setPtt(false);
    return true;
  }
  return false;
}

bool Service::processRotaryEncoder()
{
  bool shouldUpdateScreen = false;
  long encoderDelta = rotaryEncoder_->encoderChanged();

  if (encoderDelta != 0) {
    LOG_INFO("Encoder changed:", rotaryEncoder_->readEncoder(), encoderDelta);
    if (settingsMenu_) {
      settingsMenu_->onEncoderPositionChanged(encoderDelta);
      settingsMenu_->draw(display_);
    } else {
      audioTask_->changeVolume(encoderDelta);
      shouldUpdateScreen = true;
    }
    pmService_->lightSleepReset();
  }

  if (rotaryEncoder_->isEncoderButtonClicked()) {
    LOG_INFO("Encoder button clicked", esp_get_free_heap_size());
    if (settingsMenu_) {
      settingsMenu_->onEncoderButtonClicked();
      settingsMenu_->draw(display_);
    } else {
      shouldUpdateScreen = true;
    }
    pmService_->lightSleepReset();
  }

  if (rotaryEncoder_->isEncoderButtonClicked(CfgEncoderBtnLongMs)) {
    LOG_INFO("Encoder button long clicked");
    if (settingsMenu_) {
      settingsMenu_.reset();
      shouldUpdateScreen = true;
    } else {
      settingsMenu_ = std::make_shared<SettingsMenu>(config_);
      settingsMenu_->draw(display_);
    }
    pmService_->lightSleepReset();
  }

  return shouldUpdateScreen;
}

void Service::loop() 
{
  bool screenNeedsUpdate = false;

  screenNeedsUpdate |= audioTask_->loop();
  screenNeedsUpdate |= radioTask_->loop();
  screenNeedsUpdate |= pmService_->loop();
  screenNeedsUpdate |= processPttButton();
  screenNeedsUpdate |= processRotaryEncoder();

  if (screenNeedsUpdate) {
    updateScreen();
  }
}

} // LoraDv