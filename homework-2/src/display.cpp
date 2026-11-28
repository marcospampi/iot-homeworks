#include "display.h"

void DisplayUtils::setHeader(const char *header, int16_t x) {
  if(display == NULL || header == NULL)
    return;

    display->setTextSize(1);
    display->setCursor(x, 0);
    display->print(header);
}

void DisplayUtils::writeText(const char *txt, int16_t x, int16_t y, uint8_t textSize, bool autoClear, bool autoUpdate) {
  if(display == NULL)
    return;

  if(autoClear)
    display->clearDisplay();

  display->setTextSize(textSize);
  display->setCursor(x, y);

  if(txt != NULL)
    display->print(txt);

  if(autoUpdate)
    display->display();
}


void DisplayUtils::showMessage(const char *title, const char *body, int16_t titleX, int16_t bodyX) {
  if(display == NULL)
    return;

  display->clearDisplay();

  if(title != NULL && title[0] != '\0') {
    display->setTextSize(1);
    display->setCursor(titleX, 0);
    display->print(title);
  }

  if(body != NULL && body[0] != '\0') {
    display->setTextSize(2);
    display->setCursor(bodyX, 16);
    display->print(body);
  }

  display->display();
}

void DisplayUtils::writeCountdown(const char *txt, int seconds, bool autoClear, int16_t x, int16_t y, int16_t timerX, int16_t timerY) {
  if(display == NULL)
    return;

  if(seconds < 0)
    seconds = 0;

  if(autoClear)
    display->clearDisplay();

  display->setTextSize(2);
  display->setCursor(x, y);

  if(txt != NULL)
    display->print(txt);

  display->setCursor(timerX, timerY);
  display->print(seconds);
  display->print("s");

  display->display();
}

void DisplayUtils::drawProgressBar(float value, float maxValue, int16_t x, int16_t y, int16_t w, int16_t h) {
  if(display == NULL || maxValue <= 0)
    return;

  if(value < 0)
    value = 0;
  if(value > maxValue)
    value = maxValue;

  display->drawRect(x, y, w, h, 1);

  int fillW = (w - 4) * value / maxValue;
  if(fillW > 0)
    display->fillRect(x + 2, y + 2, fillW, h - 4, 1);
}
