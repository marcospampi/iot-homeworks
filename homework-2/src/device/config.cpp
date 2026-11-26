#include "device.h"
#include "ble-service.h"

#include <iot_board.h>
#include "settings.h"


class ConfigState : public DeviceState {
    public:
        void loop() override {
            if(BLEInterfaceService::can_play()) {
                set_state(DEVICE_SYNC);
            }
        }
};

DeviceState *device_state_config = new ConfigState();
