#include "device.h"
#include "semaphore.h"
class SyncMasterState : public DeviceState {
    public:
        void enter() override {
            Serial.println("Sono in")
            SemaphoreState::set_state(SEMAPHORE_RED);
        }

        void loop() override {
            // TODO
        }
};

DeviceState *device_state_sync_master = new SyncMasterState();
