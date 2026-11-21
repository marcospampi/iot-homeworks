#include <iot_board.h>

#include "semaphore.h"

#define TIME 1000

class GreenState : public SemaphoreState {
    public:
        void loop() override {
            digitalWrite(LED_RED,    LOW);
            digitalWrite(LED_YELLOW, LOW);
            digitalWrite(LED_GREEN,  HIGH);

            if(millis() - timer > TIME)
                SemaphoreState::set_state(SEMAPHORE_YELLOW);
        }
};

SemaphoreState *semaphore_state_green = new GreenState();
