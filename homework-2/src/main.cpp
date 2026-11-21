#include <iot_board.h>
#include "semaphore.h"

void setup()
{
  sleep(3);
  IoTBoard::init_serial();
  
  Serial.println("Hello world!");

  DeviceState::set_state(SEMAPHORE_CONFIG);
  SemaphoreState::set_state(SEMAPHORE_BLINKING);
}

void loop() {
    DeviceState::get_state()->loop();
    SemaphoreState::get_state()->loop();
}
