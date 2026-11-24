#include "ble-service.h"
#include <BLEDevice.h>

static BLEServer *server;

#define SERVICE_UUID "e98ba5b6-9702-45ca-94e6-8707e05959e9"
static BLEService *service;

#define PAN_ID_UUID "ed29dd09-ca4c-4e49-8f67-4cf88592da96"
static BLECharacteristic *pan_id_characteristic;

#define PEER_ID_UUID "68c79005-c3c4-404b-8023-12e5ed8684f318:35"
static BLECharacteristic *peer_id_characteristic;

#define IS_MASTER_UUID "e1f72cac-b7b5-4b88-8c18-6620715f5bbb"
static BLECharacteristic *is_master_characteristic;

#define GROUP_ID_UUID "049d3bae-54e7-4b77-b7ca-c7e20e9aa779"
static BLECharacteristic *group_id_characteristic;

#define DEVICE_COUNT_UUID "f048cbec-ba22-45f6-8b57-d7b1080979fd"
static BLECharacteristic *device_count_characteristic; // master only

#define GROUP_COUNT_UUID "4ad9f30e-f4c5-45ea-b474-da6f17e46db0"
static BLECharacteristic *group_count_characteristic; // master only

#define CAN_PLAY_UUID "bcd2aa24-9ed7-4cc2-8b51-ae0dad69df49"
static BLECharacteristic *can_play_characteristic;

#define SEMAPHORE_STATE_UUID "a2e7a989-74eb-4878-8f6d-a909eccb5617"
static BLECharacteristic *semaphore_state_characteristic;

//"a2df5f59-14da-4ccf-8253-6983626699d1"

class CharacteristicCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *c) {
        int value = c->getValue().toInt();

        // TODO
        //if(c == pan_id_characteristic) {
        //} else if(_characteristic) {

        //} else if(_characteristic) {

        //} else if(_characteristic) {

        //} else if(_characteristic) {

        //} else if(_characteristic) {

        //}
    }
} characteristic_callback;

void BLEInterfaceService::init() {
    BLEDevice::init("Gianni Vezzosi");

    server = BLEDevice::createServer();
    service = server->createService(SERVICE_UUID);

    pan_id_characteristic = service->createCharacteristic(PAN_ID_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE);
    pan_id_characteristic->setValue("");
    pan_id_characteristic->setCallbacks(&characteristic_callback);

    peer_id_characteristic = service->createCharacteristic(PEER_ID_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE);
    peer_id_characteristic->setValue("");
    peer_id_characteristic->setCallbacks(&characteristic_callback);

    is_master_characteristic = service->createCharacteristic(IS_MASTER_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE);
    is_master_characteristic->setValue("");
    is_master_characteristic->setCallbacks(&characteristic_callback);

    group_id_characteristic = service->createCharacteristic(GROUP_ID_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE);
    group_id_characteristic->setValue("");
    group_id_characteristic->setCallbacks(&characteristic_callback);

    device_count_characteristic = service->createCharacteristic(DEVICE_COUNT_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE);
    device_count_characteristic->setValue("");
    device_count_characteristic->setCallbacks(&characteristic_callback);

    group_count_characteristic = service->createCharacteristic(GROUP_COUNT_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE);
    group_count_characteristic->setValue("");
    group_count_characteristic->setCallbacks(&characteristic_callback);

    can_play_characteristic = service->createCharacteristic(CAN_PLAY_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE);
    can_play_characteristic->setValue("");
    can_play_characteristic->setCallbacks(&characteristic_callback);

    semaphore_state_characteristic = service->createCharacteristic(SEMAPHORE_STATE_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_NOTIFY);
    semaphore_state_characteristic->setValue("");
    semaphore_state_characteristic->setCallbacks(&characteristic_callback);

    service->start();
    server->start();

    BLEAdvertising *adv = BLEDevice::getAdvertising();
    adv->addServiceUUID(SERVICE_UUID);
    adv->setScanResponse(true);
}

bool BLEInterfaceService::can_play() {
    return (can_play_characteristic->getValue() == "1");
}

void BLEInterfaceService::notify_semaphore_state(const char *val) {
    semaphore_state_characteristic->setValue((const uint8_t *) val, strlen(val));
    semaphore_state_characteristic->notify();
}
