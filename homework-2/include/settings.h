#pragma once

struct Settings{
    bool is_master;
    int group_id;
    int device_count;
    int group_count;
};

extern Settings settings;
