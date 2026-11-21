#include <iot_board.h>
#include "semaphore.h"
#include "device.h"

#include "settings.h"
Settings settings; //TODO: creare un file per questo

void setup()
{
  sleep(3);
  IoTBoard::init_serial();

  Serial.println("Hello world!");

  DeviceState::set_state(DEVICE_CONFIG);
  SemaphoreState::set_state(SEMAPHORE_BLINKING);
}

void loop() {
    DeviceState::get_state()->loop();
    SemaphoreState::get_state()->loop();
}
