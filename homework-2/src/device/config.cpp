#include "device.h"

#include <iot_board.h>
#include <BLEDevice.h>
#include "settings.h"

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

class ConfigState : public DeviceState {
    private:
        BLEServer *server;

        #define SERVICE_UUID "e98ba5b6-9702-45ca-94e6-8707e05959e9"
        BLEService *service;

        #define IS_MASTER_UUID "e1f72cac-b7b5-4b88-8c18-6620715f5bbb"
        BLECharacteristic *is_master_characteristic;

        #define GROUP_ID_UUID "049d3bae-54e7-4b77-b7ca-c7e20e9aa779"
        BLECharacteristic *group_id_characteristic;

        #define DEVICE_COUNT_UUID "f048cbec-ba22-45f6-8b57-d7b1080979fd"
        BLECharacteristic *device_count_characteristic; // master only

        #define GROUP_COUNT_UUID "4ad9f30e-f4c5-45ea-b474-da6f17e46db0"
        BLECharacteristic *group_count_characteristic; // master only

    public:
        void enter() override {
            BLEDevice::init("Gianni Vezzosi");
            server = BLEDevice::createServer();
            service = server->createService(SERVICE_UUID);

            is_master_characteristic = service->createCharacteristic(IS_MASTER_UUID,
                BLECharacteristic::PROPERTY_READ |
                BLECharacteristic::PROPERTY_WRITE);
            is_master_characteristic->setValue("");
            is_master_characteristic->setCallbacks(new IsMasterCallback());

            group_id_characteristic = service->createCharacteristic(GROUP_ID_UUID,
                BLECharacteristic::PROPERTY_READ |
                BLECharacteristic::PROPERTY_WRITE);
            group_id_characteristic->setValue("");
            group_id_characteristic->setCallbacks(new GroupIdCallback());

            device_count_characteristic = service->createCharacteristic(DEVICE_COUNT_UUID,
                BLECharacteristic::PROPERTY_READ |
                BLECharacteristic::PROPERTY_WRITE);
            device_count_characteristic->setValue("");
            device_count_characteristic->setCallbacks(new DeviceCountCallbacks());

            group_count_characteristic = service->createCharacteristic(GROUP_COUNT_UUID,
                BLECharacteristic::PROPERTY_READ |
                BLECharacteristic::PROPERTY_WRITE);
            group_count_characteristic->setValue("");
            group_count_characteristic->setCallbacks(new GroupCountCallbacks());

            service->start();
            server->start();

            BLEAdvertising *adv = BLEDevice::getAdvertising();
            adv->addServiceUUID(SERVICE_UUID);
            adv->setScanResponse(true);

            BLEDevice::startAdvertising();
        }

        void loop() override {
            bool is_master_set    = (is_master_characteristic->getLength() > 0);
            bool group_id_set     = (group_id_characteristic->getLength() > 0);
            bool device_count_set = (device_count_characteristic->getLength() > 0);
            bool group_count_set  = (group_count_characteristic->getLength() > 0);

            // check if 'is_master' and 'group_id' are set
            if(!is_master_set || !group_id_set)
                return;

            if(settings.is_master) {
                // check if 'device_count' and 'group_count' are set
                if(!device_count_set || !group_count_set)
                    return;
                set_state(DEVICE_SYNC_MASTER);
            } else {
                set_state(DEVICE_SYNC_SLAVE);
            }
        }
};

DeviceState *device_state_config = new ConfigState();
