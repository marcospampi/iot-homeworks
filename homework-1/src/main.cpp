#include <iot_board.h>

void setup()
{
  sleep(3);
  IoTBoard::init_serial();
  
  Serial.println("Hello world!");
}

void loop() {}
