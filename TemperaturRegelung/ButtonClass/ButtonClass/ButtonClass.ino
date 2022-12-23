/*-------------------------------------------------------------------------
Temperatur Regelung Menü
Version: V0.5
created: 29.01.2021
last edit: 23.12.2022
author: Jonathan Schumann
mail: jonathanschumann@gmx.de


Hier ist die Button Class

// Versionsnummer
const char VersNr[8] = "V0.9.0";

-------------------------------------------------------------------------*/

// Zeit
unsigned long TimeOfLastInput;  // global variabel to keep trak of last action
const unsigned int DisplayStandBy = 30000;  // Time until backlight goes to sleep 
const unsigned int MenuReturnTime = 10000;  // Time until cancel is triggered automatically


//misc
char timearray[9]; // here runtime is saved in format dd:hh:mm
unsigned long Cyclen = 0;  // Anzahl der Einschaltvorgänge


class Button {
  private:
    byte pin;
    byte state;
    byte lastReading;
    unsigned long lastDebounceTime = 0;
    unsigned long debounceDelay = 75;
  public:
    Button(byte pin) {
      this->pin = pin;
      lastReading = LOW;
      init();
    }
    void init() {
      pinMode(pin, INPUT_PULLUP);
      update();
    }
    void update() {
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
    byte getState() {
      update();
      return state;
    }
    bool isPressed() {
      return (getState() == LOW);
    }
};  // don't forget the semicolon at the end of the class
