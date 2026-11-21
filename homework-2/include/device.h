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

#define DEVICE_SYNC_SLAVE  device_state_sync_slave
#define DEVICE_SYNC_MASTER device_state_sync_master

#define DEVICE_LOOP_SLAVE  device_state_loop_slave
#define DEVICE_LOOP_MASTER device_state_loop_master

extern DeviceState
    *device_state_config,
    *device_state_sync_slave,
    *device_state_sync_master,
    *device_state_loop_slave,
    *device_state_loop_master;
