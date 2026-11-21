#include "device.h"

class SyncSlaveState : public DeviceState {
    public:
        void enter() override {
            // TODO
        }

        void loop() override {
            // TODO
        }
};

DeviceState *device_state_sync_slave = new SyncSlaveState();
