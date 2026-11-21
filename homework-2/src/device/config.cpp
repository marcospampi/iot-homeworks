#include "device.h"

#include <iot_board.h>
#include <BLEDevice.h>

class ConfigState : public DeviceState {
    private:
        BLEServer  *server;
        BLEService *service;

        #define IS_MASTER_UUID ""
        BLECharacteristic *is_master;

        #define GROUP_ID_UUID ""
        BLECharacteristic *group_id;

        #define DEVICE_COUNT_UUID ""
        BLECharacteristic *device_count; // master only

        #define GROUP_COUNT_UUID ""
        BLECharacteristic *group_count;  // master only

    public:
        void enter() override {
            BLEDevice::init("Gianni Vezzosi");
            server = BLEDevice::createServer();
            service = server->createService(SERVICE_UUID);

            is_master = service->createCharacteristic(IS_MASTER_UUID,
                BLECharacteristic::PROPERTY_READ |
                BLECharacteristic::PROPERTY_WRITE);
            is_master->setValue(-1);
            // TODO callback

            group_id = service->createCharacteristic(GROUP_ID_UUID,
                BLECharacteristic::PROPERTY_READ |
                BLECharacteristic::PROPERTY_WRITE);
            group_id->setValue(-1);
            // TODO callback

            device_count = service->createCharacteristic(DEVICE_COUNT_UUID,
                BLECharacteristic::PROPERTY_READ |
                BLECharacteristic::PROPERTY_WRITE);
            group_id->setValue(-1);
            // TODO callback

            group_count = service->createCharacteristic(GROUP_COUNT_UUID,
                BLECharacteristic::PROPERTY_READ |
                BLECharacteristic::PROPERTY_WRITE);
            group_count->setValue(-1);
            // TODO callback

  //ledChar->setCallbacks(new LedCallbacks());
            service->start();
            server->start();

            BLEAdvertising *adv = BLEDevice::getAdvertising();
            adv->addServiceUUID(SERVICE_UUID);
            adv->setScanResponse(true);

            BLEDevice::startAdvertising();
        }

        void loop() override {

        }
};

DeviceState *device_state_config = new ConfigState();

// BLEDevice
