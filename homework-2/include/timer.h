#pragma once

#include <stdint.h>

class Timer {
private:
    int interval;             //tempo da attendere in millisecondi
    int previous_millis;      //tempo di inizio del timer
    bool auto_reset;          //fa ricominciare il timer una volta terminato

public:
    explicit Timer(int _interval, bool _auto_reset = false);    //crea un nuovo timer

    void reset(int new_interval = 0);                           //setta un nuovo tempo al timer
    int elapsed() const;                                        //quanto tempo é passato
    int remaining() const;                                      //quanto tempo resta al timer
    bool operator()();
};
