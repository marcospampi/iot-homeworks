#pragma once

class BLEService {
    public:
        static void init();

        // false = stay in CONFIG, true = move to SYNC/LOOP
        static bool can_play();

        static void notify_semaphore_state(const char *val);
};
