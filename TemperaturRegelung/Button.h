#ifndef MY_BUTTON_H
#define MY_BUTTON_H

#include <Arduino.h>


unsigned long TimeOfLastInput = 0;

class Button {

  private:
    byte pin;
    byte state;
    byte lastReading;
    unsigned long lastDebounceTime = 0;
    unsigned long debounceDelay = 75;

  public:
    Button(byte pin);

    void init();
    void update();

    byte getState();
    bool isPressed();
};

#endif
