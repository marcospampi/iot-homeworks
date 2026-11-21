#include <iot_board.h>

#include "semaphore.h"

#define TIME 1000

class YellowState : public SemaphoreState {
    public:
        void loop() override {
            digitalWrite(LED_RED,    LOW);
            digitalWrite(LED_YELLOW, HIGH);
            digitalWrite(LED_GREEN,  LOW);

            if(millis() - timer > TIME)
                SemaphoreState::set_state(SEMAPHORE_RED);
        }
};

SemaphoreState *semaphore_state_yellow = new YellowState();
