#include <Button.h>

    Button::Button(byte pin) {
      this->pin = pin;
      lastReading = LOW;
      init();
    }
    void Button::init() {
      pinMode(pin, INPUT_PULLUP);
      update();
    }
    void Button::update() {
      // You can handle the debounce of the button directly
      // in the class, so you don't have to think about it
      // elsewhere in your code
      byte newReading = digitalRead(pin);
      
      if (newReading != lastReading) {
        lastDebounceTime = millis();
        TimeOfLastInput = lastDebounceTime;
      }
      if (millis() - lastDebounceTime > debounceDelay) {
        // Update the 'state' attribute only if debounce is checked
        state = newReading;
      }
      lastReading = newReading;
    }
    byte Button::getState() {
      update();
      return state;
    }
    bool Button::isPressed() {
      return (getState() == LOW);
    }
