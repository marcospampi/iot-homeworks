#include "device.h"
#include "ble-service.h"
#include "zigbee-service.h"
#include "semaphore.h"
#include "settings.h"

class LoopState : public DeviceState {
    private:

        int group_counter = 0;

        bool accept(){
            if(BLEInterfaceService::can_play() == false){
                set_state(DEVICE_CONFIG);
                return false;
            }

            if(!ZigBeeService::all_heartbeats_ok()){
                set_state(DEVICE_SYNC);
                return false;
            }
            ZigBeeService::send_heartbeat();
            return true;
        }
    public:
        void enter() override {
            group_counter = 0;
            SemaphoreState::set_state(SEMAPHORE_RED);
        }

        void loop() override {
            if(!accept()) return;

            bool go_up = ZigBeeService::should_go_up();

            if(settings.is_master){
                if(IoTBoard::runEvery(SEMAPHORE_LOOP_TIME,2)){
                    ZigBeeService::send(MESSAGE_GO,group_counter);
                    go_up = (group_counter == settings.group_id);
                    group_counter = (group_counter + 1) % settings.group_count;
                }
            }
            if(go_up)
                SemaphoreState::set_state(SEMAPHORE_GREEN);
        }
};

DeviceState *device_state_loop = new LoopState();
