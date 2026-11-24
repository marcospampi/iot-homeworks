#include "device.h"
#include "semaphore.h"
#include "zigbee-service.h"
#include "ble-service.h"
#include "settings.h"

class SyncState : public DeviceState {
    public:
        void enter() override {
            Serial.println("Sono in");
            SemaphoreState::set_state(SEMAPHORE_BLINKING);
        }

        void loop() override {
            if(BLEInterfaceService::can_play() == false){
                set_state(DEVICE_CONFIG);
                return;
            }

            ZigBeeService::send_heartbeat();
            if(ZigBeeService::all_heartbeats_ok()){
                set_state(DEVICE_LOOP);
            }
        }
};

DeviceState *device_state_sync = new SyncState();
