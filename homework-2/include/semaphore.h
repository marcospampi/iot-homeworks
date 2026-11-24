#pragma once

#include <iot_board.h>

#define SEMAPHORE_RED_TIME 1000
#define SEMAPHORE_YELLOW_TIME 1000
#define SEMAPHORE_GREEN_TIME 1000
#define SEMAPHORE_LOOP_TIME (SEMAPHORE_RED_TIME + SEMAPHORE_YELLOW_TIME + SEMAPHORE_GREEN_TIME)

class SemaphoreState {
    private:
        static SemaphoreState *current_state;

    protected:
        int timer;

        // TODO implement this on states
        virtual void enter() {
            // do nothing
        }

    public:
        static void set_state(SemaphoreState *new_state) {
            current_state = new_state;
            current_state->timer = millis();
            current_state->enter();
        }

        static SemaphoreState *get_state() {
            return current_state;
        }

        virtual void loop() = 0;
};

#define SEMAPHORE_GREEN    semaphore_state_green
#define SEMAPHORE_YELLOW   semaphore_state_yellow
#define SEMAPHORE_RED      semaphore_state_red
#define SEMAPHORE_BLINKING semaphore_state_blinking

extern SemaphoreState
    *semaphore_state_green,
    *semaphore_state_yellow,
    *semaphore_state_red,
    *semaphore_state_blinking;
