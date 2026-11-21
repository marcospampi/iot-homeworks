#include "device.h"

class SyncMasterState : public DeviceState {
    public:
        void enter() override {
            // TODO
        }

        void loop() override {
            // TODO
        }
};

DeviceState *device_state_sync_master = new SyncMasterState();
