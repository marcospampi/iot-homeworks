#include "device.h"

class LoopMasterState : public DeviceState {
    public:
        void enter() override {
            // TODO
        }

        void loop() override {
            // TODO
        }
};

DeviceState *device_state_loop_master = new LoopMasterState();
