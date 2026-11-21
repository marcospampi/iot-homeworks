#include "device.h"

class LoopSlaveState : public DeviceState {
    public:
        void enter() override {
            // TODO
        }

        void loop() override {
            // TODO
        }
};

DeviceState *device_state_loop_slave = new LoopSlaveState();
