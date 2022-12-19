#include "loradv_service.h"

LoraDv::Service loraDvService_;

void initializeConfig(LoraDv::Config &config) 
{
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  while (!Serial);

  LoraDv::Config config;
  initializeConfig(config);

  loraDvService_.setup(config);
}

void loop() {
  loraDvService_.loop();
  delay(50);
}

