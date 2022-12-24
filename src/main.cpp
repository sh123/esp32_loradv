#include <Arduino.h>
#include <memory>

#include "loradv_service.h"

const int LoopDelayMs = 50;

LoraDv::Service loraDvService_;
std::shared_ptr<LoraDv::Config> config_;

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  while (!Serial);

  config_ = std::make_shared<LoraDv::Config>();
  config_->Load();
  
  loraDvService_.setup(config_);
}

void loop() {
  loraDvService_.loop();
  delay(LoopDelayMs);
}

