#pragma once
#include <iot_board.h>
struct Settings{
    bool is_master;
    int group_id;
    int device_count;
    int group_count;
    
    uint16_t pan_id;
    uint16_t peer_id;
};

extern Settings settings;
