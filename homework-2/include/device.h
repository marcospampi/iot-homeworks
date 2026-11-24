#pragma once

class DeviceState {
    private:
        static DeviceState *current_state;

    protected:
        virtual void enter() {
            // do nothing
        }

    public:
        static void set_state(DeviceState *new_state) {
            current_state = new_state;
            current_state->enter();
        }

        static DeviceState *get_state() {
            return current_state;
        }

        virtual void loop() = 0;
};

#define DEVICE_CONFIG device_state_config
#define DEVICE_SYNC device_state_sync
#define DEVICE_LOOP device_state_loop

extern DeviceState
    *device_state_config,
    *device_state_sync,
    *device_state_loop;
