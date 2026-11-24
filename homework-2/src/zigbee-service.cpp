#include "zigbee-service.h"

#include <iot_board.h>

#define MAX_PEERS 16
#define ID_OFFSET 0x0001

#define HEARTBEAT_TRESHOLD 1000
#define HEARTBEAT_FREQUENCY 4 // 4 Hz = 250 ms

struct Message {
    uint8_t msgtype;
    uint8_t data;
};

static volatile int last_up_direction = -1;
static volatile bool up_fresh = false;

// keep track of last time we heard from devices
//
// NOTE: master MUST have ID = ID_OFFSET
static volatile int timestamp[MAX_PEERS];

void on_zigbee_receive(rx_info_t *packet) {
    // TODO check if pan_id == settings.pan_id

    // TODO ...
    // uint16_t from = packet->src_addr;

    // TODO
    // if(msgtype == MESSAGE_GO) {
    //   last_up_direction = data
    //   up_fresh = true
    // }

    // TODO ho ricevuto un messaggio (HEARTBEAT o GO...)
    // aggiorna timestamp di 'from'
    // TODO importante controllate che
    //     ID_OFFSET <= from < ID_OFFSET + MAX_PEERS
}

void ZigBeeService::init() {
    zigbee->onReceiveCallback(on_zigbee_receive);
}

void ZigBeeService::set_pan_id(int id) {
    zigbee->setPanId(id);
}

void ZigBeeService::set_peer_id(int id) {
    zigbee->setAddress(id);
}

void ZigBeeService::set_channel(int channel) {
    zigbee->setChannel(DEFAULT_CHANNEL);
}

bool ZigBeeService::all_heartbeats_ok() {
    int expected_count = settings.is_master ? settings.device_count : 1;

    int curr_time = millis();
    for(int i = 0; i < expected_count; ++i) {
        if (curr_time - timestamp[i - ID_OFFSET] < HEARTBEAT_TRESHOLD)
            return false;
    }
    return true;
}

// TODO create a message queue
static void send_packet(int id, const struct Message *msg) {
    zigbee->startPacket();
    zigbee->writeUnsignedBytes(msg, sizeof(struct Message));
    zigbee->sendPacket(id, true);

    // TODO
    // start_time = millis()
    // while( !transmissionDone && (millis() - start_time < timeout))
    //     ...
    // return transmissionDone
    return true;
}

void ZigBeeService::send(uint8_t msgtype, uint8_t data) {
    const struct Message msg = { msgtype, data };

    if(settings.master) {
        // send to all slaves
        for(int i = 1; i < settings.device_count; i++) {
            send_packet(ID_OFFSET + i, &msg);
        }
    } else {
        // send only to master
        send_packet(ID_OFFSET, &msg);
    }
}

void ZigBeeService::send_heartbeat() {
    if(IoTBoard::runEvery(1000 / HEARTBEAT_FREQUENCY, 7)
        send(MESSAGE_HEARTBEAT, 0);
}

bool should_go_up(int group_id) {
    if(up_fresh) {
        up_fresh = false;
        return (group_id == last_up_direction);
    }
    return false;
}
