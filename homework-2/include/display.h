#pragma once

#include <iot_board.h>

namespace DisplayUtils {

    void setHeader(const char *header = "", int16_t x = 0);
    void writeText(const char *txt = "", int16_t x = 0, int16_t y = 16, uint8_t textSize = 2, bool autoClear = false, bool autoUpdate = true);
    void showMessage(const char *title, const char *body = "", int16_t titleX = 0, int16_t bodyX = 0);
    void writeCountdown(const char *txt, int seconds, bool autoClear=false, int16_t x = 30, int16_t y = 16, int16_t timerX = 50, int16_t timerY = 36);
    void drawProgressBar(float value, float maxValue, int16_t x = 4, int16_t y = 56, int16_t w = 120, int16_t h = 8);

};
