#include <Arduino.h>
#include <memory>

#include "service.h"

using namespace LoraDv;

static constexpr int LoopDelayMs = 10;

std::shared_ptr<Config> config_;
std::shared_ptr<Service> service_;

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  while (!Serial);

  config_ = std::make_shared<Config>();
  config_->Load();
  
  service_ = std::make_shared<Service>(config_);
  service_->setup();
}

void loop() {
  service_->loop();
  delay(LoopDelayMs);
}

