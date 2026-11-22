#include "device.h"
#include "semaphore.h"
#include "messages.h"
#include "heartbeat.h"
class SyncMasterState : public DeviceState {
    public:
        void enter() override {
            SemaphoreState::set_state(SEMAPHORE_RED);
        }

        void loop() override {
            
            if( IoTBoard::runEvery(500, 3) ) {
                send_advertising();
            }
            
            if ( Heartbeat::ok() ) {
                // TODO: send GO ( ALL_RED )
                return set_state(DEVICE_LOOP_MASTER);
            }
        }

    private:
        void send_advertising() {
            Message adv;
            adv.type = MESSAGE_ADVERTISING;
            zigbee->startPacket();
            zigbee->writeBytes((uint8_t*)&adv, sizeof(adv));
            zigbee->sendPacket(0xFFFF, false);
        }
};

DeviceState *device_state_sync_master = new SyncMasterState();
