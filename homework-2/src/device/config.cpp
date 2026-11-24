#include "device.h"
#include "ble-service.h"

#include <iot_board.h>
#include "settings.h"

// TODO remove all these
#if 0
class PanIdCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *c) {
        int value = c->getValue().toInt();
        settings.pan_id = value;
        Serial.printf("PanId = %d", value);
    }
};

class PeerIdCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *c) {
        int value = c->getValue().toInt();
        settings.peer_id = value;
        Serial.printf("PeerId = %d", value);
    }
};

class IsMasterCallback : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *c) {
        int value = c->getValue().toInt();
        settings.is_master = value;
        Serial.printf("IsMaster = %d", value);
    }
};

class GroupIdCallback : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *c) {
        int value = c->getValue().toInt();
        settings.group_id = value;
        Serial.printf("GroupId = %d", value);

    }
};

class DeviceCountCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *c) {
        int value = c->getValue().toInt();
        settings.device_count = value;
        Serial.printf("DeviceCount = %d", value);

    }
};

class GroupCountCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *c) {
        int value = c->getValue().toInt();
        settings.group_count = value;
        Serial.printf("GroupCount = %d", value);
    }
};
#endif

class ConfigState : public DeviceState {
    public:
        void loop() override {
            if(BLEInterfaceService::can_play()) {
                set_state(DEVICE_SYNC);
            }
        }
};

DeviceState *device_state_config = new ConfigState();
