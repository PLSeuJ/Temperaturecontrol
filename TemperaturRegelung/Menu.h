#ifndef MENU_H
#define MENU_H

#include <Arduino.h>

class Menu {
  private:
    byte _state;
  public:
    Menu();
    void init();
    void build();
    //void update();
    byte getState();
};

#endif
