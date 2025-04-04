#include <Arduino.h>
#include <memory>

#include "loradv_service.h"

static constexpr int LoopDelayMs = 10;

std::shared_ptr<LoraDv::Service> loraDvService_;
std::shared_ptr<LoraDv::Config> config_;

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  while (!Serial);

  config_ = std::make_shared<LoraDv::Config>();
  config_->Load();
  
  loraDvService_ = std::make_shared<LoraDv::Service>(config_);
  loraDvService_->setup();
}

void loop() {
  loraDvService_->loop();
  delay(LoopDelayMs);
}

