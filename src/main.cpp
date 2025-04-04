#include <Arduino.h>
#include <memory>

#include "service.h"

static constexpr int LoopDelayMs = 10;

std::shared_ptr<LoraDv::Config> config_;
std::shared_ptr<LoraDv::Service> service_;

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  while (!Serial);

  config_ = std::make_shared<LoraDv::Config>();
  config_->Load();
  
  service_ = std::make_shared<LoraDv::Service>(config_);
  service_->setup();
}

void loop() {
  service_->loop();
  delay(LoopDelayMs);
}

