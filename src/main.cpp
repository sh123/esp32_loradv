#include <Arduino.h>
#include <memory>

#include "service.h"

using namespace std;
using namespace LoraDv;

static constexpr int LoopDelayMs = 10;

shared_ptr<Config> config_;
shared_ptr<Service> service_;

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  while (!Serial);

  config_ = make_shared<Config>();
  config_->Load();
  
  service_ = make_shared<Service>(config_);
  service_->setup();
}

void loop() {
  service_->loop();
  delay(LoopDelayMs);
}

