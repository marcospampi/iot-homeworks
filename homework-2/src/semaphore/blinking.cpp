#include <iot_board.h>

#include "semaphore.h"

#define TIME 1000

class BlinkingState : public SemaphoreState {
    private:
        bool blink_flag = false;

    public:
        void loop() override {
            digitalWrite(LED_RED,    LOW);
            digitalWrite(LED_YELLOW, blink_flag);
            digitalWrite(LED_GREEN,  LOW);

            int now = millis();
            if(now - timer > TIME) {
                blink_flag = !blink_flag;
                timer = now;
            }
        }
};

SemaphoreState *semaphore_state_blinking = new BlinkingState();
