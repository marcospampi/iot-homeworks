#include <iot_board.h>
#include "semaphore.h"
#include "device.h"
#include "settings.h"
#include "heartbeat.h"

void setup()
{
  sleep(3);
  IoTBoard::init_leds();/// uwu
  IoTBoard::init_serial();
  IoTBoard::init_display();
  IoTBoard::init_spi();
  IoTBoard::init_zigbee();

  BLEService::init();
  ZigBeeService::init();

  DeviceState::set_state(DEVICE_CONFIG);
  SemaphoreState::set_state(SEMAPHORE_BLINKING);
}

void loop() {
  DeviceState::get_state()->loop();
  SemaphoreState::get_state()->loop();

  if(IoTBoard::runEvery(500), ) {
    display->clearDisplay();
    display->setCursor(0,0);
    display->printf("is_master: %d\n", settings.is_master);
    display->printf("group_id: %d\n", settings.group_id);
    display->printf("device_count: %d\n", settings.device_count);
    display->printf("group_count: %d\n", settings.group_count);
    display->display();
  }

}
