#pragma once

extern struct Settings {
    // ZigBee settings
    int pan_id;
    int peer_id;

    bool is_master;
    int group_id;

    // master only
    int device_count;
    int group_count;
} settings;
