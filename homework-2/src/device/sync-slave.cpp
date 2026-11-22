#include "device.h"
#include "semaphore.h"
class SyncSlaveState : public DeviceState {
    public:
        void enter() override {
            SemaphoreState::set_state(SEMAPHORE_GREEN);
        }

        void loop() override {
            // TODO
        }
};

DeviceState *device_state_sync_slave = new SyncSlaveState();
