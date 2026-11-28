#include "timer.h"
#include "iot_board.h"

Timer::Timer(int _interval, bool _autoReset)
    : interval(_interval), previous_millis(millis()), auto_reset(_autoReset) {}

void Timer::reset(int new_interval) {
    if (new_interval > 0) {
        interval = new_interval;
    }
    previous_millis = millis();
}

int Timer::elapsed() const {
    return millis() - previous_millis;
}

int Timer::remaining() const {
    int _elapsed = elapsed();
    return (_elapsed >= interval) ? 0 : (interval - _elapsed);
}

bool Timer::operator()() {
    if (elapsed() >= interval) {
        if (auto_reset) {
            previous_millis = millis();
        }
        return true;
    }
    return false;
}
