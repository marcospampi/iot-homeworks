#pragma once

#include <iot_board.h>

enum MessageType {
    MESSAGE_HEARTBEAT, // heatbeat di client e server
    MESSAGE_GO         // comando accendere i semafori
};

class ZigBeeService {
    public:
        static void init();

        static void set_pan_id(int id);
        static void set_peer_id(int id);
        static void set_channel(int channel);

        static bool all_heartbeats_ok();

        static void send(uint8_t msgtype, uint8_t data);
        static void send_heartbeat();

        static bool should_go_up();
};
