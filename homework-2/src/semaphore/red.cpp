#include <iot_board.h>

#include "semaphore.h"

class RedState : public SemaphoreState {
    public:
        void loop() override {
            digitalWrite(LED_RED,    HIGH);
            digitalWrite(LED_YELLOW, LOW);
            digitalWrite(LED_GREEN,  LOW);
        }
};

SemaphoreState *semaphore_state_red = new RedState();
