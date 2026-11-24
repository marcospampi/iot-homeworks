#include "device.h"
#include "ble-service.h"
#include "zigbee-service.h"
#include "semaphore.h"
#include "settings.h"

class LoopState : public DeviceState {
    private:
        int group_counter = 0;

        bool accept() {
            // did a BLE message tell us to go back to CONFIG state?
            if(BLEInterfaceService::can_play() == false) {
                set_state(DEVICE_CONFIG);
                return false;
            }

            // are all the heartbeats ok?
            if(!ZigBeeService::all_heartbeats_ok()) {
                set_state(DEVICE_SYNC);
                return false;
            }

            ZigBeeService::send_heartbeat();
            return true;
        }

        void loop_master() {
            // every so often, increment group counter
            if(IoTBoard::runEvery(SEMAPHORE_LOOP_TIME, 2)) {
                ZigBeeService::send(MESSAGE_GO, group_counter);
                if(group_counter == settings.group_id)
                    SemaphoreState::set_state(SEMAPHORE_GREEN);

                group_counter = (group_counter + 1) % settings.group_count;
            }
        }

        void loop_slave() {
            if(ZigBeeService::should_go_up())
                SemaphoreState::set_state(SEMAPHORE_GREEN);
        }
    public:
        void enter() override {
            SemaphoreState::set_state(SEMAPHORE_RED);
            group_counter = 0;
        }

        void loop() override {
            // check if we should continue staying in this state
            if(!accept()) return;

            if(settings.is_master)
                loop_master();
            else
                loop_slave();
        }
};

DeviceState *device_state_loop = new LoopState();
