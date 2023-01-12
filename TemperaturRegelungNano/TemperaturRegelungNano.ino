/*-------------------------------------------------------------------------
Temperatur Regelung 
Version: V1.0.03
created: 29.01.2021
last edit: 06.01.2023
author: Jonathan Schumann
mail: jonathanschumann@posteo.de

for an english version contact the author please.

Das Sketch ist für einen Arduino Nano geschrieben.

Das Sketch lässt über eine Solltemperatur über Knöpfe und das LC-Display 
einstellen, die dann mit einer Heitzung versucht wird zu erreichen. Mit 
den Daten vom Thermometer werden die Ein- und Ausschaltvorgänge gesteuert.

Anschluss der Hardware:
Neben den definierten Pins, müssen das Relais und die Heitzlampe seperat 
mit Strom versorgt werden. Das Thermometer ist mit Ground und 5V zu 
versorgen. Der Daten Pin des Thermometers (Pin 2) ist über 5 KOhm mit 
5V zu verbinden. - Denn solange es sich um das DS18xx handelt gilt:
  "You will need a pull-up resistor of about 5 KOhm between the 1-Wire 
  data line and your 5V power."

Das I2C-Terminal des LC-Display ist ebenfalls mit Ground und 5V, sowie 
A4 mit SDA (data) und A5 mit SCL (clock) zu verbinden.


Für weitere Informationen bitte das README lesen.

Der Verkauf des Programms oder Teile von diesem ist nicht gestattet.
-----------------------------------------------------------------------*/
#include <OneWire.h>
#include <LCD_I2C.h>
#include <DallasTemperature.h>

// Versionsnummer
const char VersNr[8] = "V1.0.03";

// Zuweisung der Anschlüsse
const int ONE_WIRE_BUS = 2;
const int SensorPin = 2;  // Digital Pin für digitales Eingangssignal
const int PowerPin = 7;   // Digital Pin für digitales Ausgangssignal
const int LED_BUILDIN = 13;  // This is the buildin led pin too
//const int VanClockPin = 9;
//const int VanControlPin = 10;

LCD_I2C lcd(0x27);  // A4 - SDA und A5 - SCL

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensor(&oneWire);

// Globale Variablen
// Logic Variables
bool controlerstate = false;  // ON/OFF toggle
bool PowerState = 0;  // relais power on/off internal variable

// Temperatur
float MesswertTemperatur = 0;  // Rohwert für Temperatur
float Temperatur = 0;          // Temperatur in °C
float Temp_On = 28;            // Einschalttemperatur für Lampe in °C
float Temp_Off = 30;           // Ausschalttemperatur für Lampe in °C
float setTemp_Off = Temp_Off;
float setTemp_On = Temp_On;
// Temp_Off = Temp_Soll + Temp_Delta;  // <--Wenn Wärmeübergang bekannt kann so der Aufheitzvorgang beschleunigt werden

// Zeit
unsigned long Messintervall = 10 * 1000;  // in Sekunden x 1000ms/sek
unsigned long lastMeasurTime = -999999;  // Time of last measurnment to avoid measurnments on every cycle and slow down menu control
unsigned long TimeNow;
unsigned long menu_entry_time;              // when was the menuframe entered?
unsigned long TimePrev = -9999;             // force imediate measurenment
unsigned long TimeOfLastInput;              // global variabel to keep trak of last action
unsigned long TimeBacklight;              // global variabel to avoid input on black screen
const unsigned int DisplayStandBy = 30000;  // turn of backgroundlight after 30 sek * 1000ms/sek
const unsigned int MenuEntryDelay = 800; // time buttons dont work after new menuframe

//misc
char timearray[12];        // here runtime is saved in format dd:hh:mm:ss
unsigned long Cyclen = 0;  // Anzahl der Einschaltvorgänge

//menu related variables
int lastframe = 999;  // frame index to togle setup and update functions in menu appropreatly
int menutitle = 0;  // current menu index
int priormenu = 999;  // prior menu frame index


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
    return !state;
  }
  bool isPressed() {
    return (getState() == LOW);
  }
};  // don't forget the semicolon at the end of the class


Button Button_up(3);  // 3 or 4
Button Button_down(4);  // 4 or 5
Button Button_ok(6);  // 6 or 6
Button Button_cancel(5);  // 5 or 3


void lcdBacklight(void)
/* toggles LCD Backlight */
{
  if (TimeNow - TimeOfLastInput >= DisplayStandBy) {
    lcd.noBacklight();
  } else {
    lcd.backlight();
    TimeBacklight = TimeNow;
  }
}

//Programm start
void setup() {
  Serial.begin(19200);  // Serielle Schnittstelle starten für einfacheres testen

  sensor.begin();  // Temperatursensor konfigurieren

  lcd.begin();      // startet den LCD Bidschirm
  lcd.clear();      // Bildschirmausgaben löschen und Curor auf 0,0 setzen
  lcd.backlight();  // Hintergrundbeleuchtung einschalten
  lcd.print(VersNr);

  pinMode(SensorPin, INPUT);
  pinMode(PowerPin, OUTPUT);
  pinMode(PowerPin, LOW);
  pinMode(LED_BUILDIN, OUTPUT);

  TimeOfLastInput = millis();  // initiallize time

  delay(1000);
  lcd.clear();  // Bildschirmausgaben löschen und Curor auf 0,0 setzen
}



float Messung(void)
/* Misst Temperatur und gibt Temperatur in °C zurück. */
{
  sensor.requestTemperatures();                // Temperatur anfragen
  MesswertTemperatur = analogRead(SensorPin);  // Temperatur auslesen...
  return sensor.getTempCByIndex(0);            // ...und in °C speichern
}


void Heater(bool PowerState) {
  pinMode(PowerPin, PowerState);

  // lcd.setCursor(10, 1);
  // if (PowerState) {
  //   lcd.print("on ");
  // } else {
  //   lcd.print("off");
  // }
}


void TimeToString(unsigned long millis, char seperator = ":")  // make void a chat[12] Time
/*returns Time in string in dd:hh:mm:ss format */
{
  int sec = millis / 1000 % 60;
  int min = millis / 60000 % 60;
  int hrs = millis / (60 * 60 * 1000) % 24;
  int day = millis / (24 * 60 * 60 * 1000);

  Serial.print(day);
  Serial.print(hrs);
  Serial.print(min);
  Serial.println(sec);

  //return Time
}

// LCD Menu Frames -------------------------------------------------

void LCDbuildMain(void) {
  lcd.clear();
  lcd.print(Temp_Off, 1);
  lcd.print((char)223);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print(Temp_On, 1);
  lcd.print((char)223);
  lcd.print("C ");

  lcd.setCursor(14,1);
  lcd.print((char)223);
  lcd.print("C ");
}


void LCDupdateMain(void) {
  bool prevState;
  float prevTemp;

  lcd.setCursor(13, 0);
  if (controlerstate) {
    lcd.print(" ON");    
  } else {
    lcd.print("OFF");
  }
  
  lcd.setCursor(8, 1);    
  if (prevState != PowerState) {
    prevState = PowerState;
      if (PowerState) {
        lcd.print("^");
      } else {
        lcd.print("v");
      }
  } else if (!controlerstate) {
    lcd.print(" ");
  }

  lcd.setCursor(10, 1);
  if (abs(Temperatur - prevTemp) >= 0.1) {
    prevTemp = Temperatur;
    lcd.print(Temperatur, 1);
    lcd.print((char)223);
    lcd.print("C");
  }
}


void LCDbuildSetTemp(int select) {
  lcd.clear();
  lcd.print(" ");
  lcd.print(setTemp_Off, 1);
  lcd.print((char)223);
  lcd.print("C");

  lcd.setCursor(1, 1);
  lcd.print(setTemp_On, 1);
  lcd.print((char)223);
  lcd.print("C ");

  lcd.setCursor(0, select);
  lcd.print(">");
  delay(100);  // avoid direct button response
}


void LCDupdateSetTemp(int select) {
  if (setTemp_Off < setTemp_On) {
    switch (select) {
      case 0:
        setTemp_On = setTemp_Off;
        break;
      case 1:
        setTemp_Off = setTemp_On;
        break;
    }
  }
  lcd.setCursor(1, 0);
  lcd.print(setTemp_Off, 1);
  lcd.setCursor(1, 1);
  lcd.print(setTemp_On, 1);
}


void LCDbuildConfirm() {
  lcd.clear();
  lcd.print("aus: ");
  lcd.print(setTemp_Off, 1);
  lcd.print((char)223);
  lcd.print("C ?");

  lcd.setCursor(0, 1);
  lcd.print(" an: ");
  lcd.print(setTemp_On, 1);
  lcd.print((char)223);
  lcd.print("C ?");
}


// Programm ---------------------------------------------------
void loop() {
  Button_ok.update();
  Button_up.update();
  Button_down.update();
  Button_cancel.update();

  switch (menutitle) {
    case 0:  // mainmenu
      if (lastframe != menutitle) {
        LCDbuildMain();
        lastframe = menutitle;
        menu_entry_time = millis();
        setTemp_Off = Temp_Off;
        setTemp_On = Temp_On;
        LCDupdateMain();
      } else if ((TimeNow - menu_entry_time > MenuEntryDelay) && (TimeNow - TimeBacklight < MenuEntryDelay)) {
        lcd.setCursor(10,0);
        if (Button_ok.getState()) { // --> Mainmenu
          controlerstate = true;
          // lcd.print("ok   ");
        } else if (Button_down.getState()) { // --> set lower Temp
          menutitle = 2;
          // lcd.print("down ");
        } else if (Button_up.getState()) { // --> set upper Temp
          menutitle = 1;
          // lcd.print("up   ");
        } else if (Button_cancel.getState()) { // --> Mainmenu
          controlerstate = false;
          // lcd.print("clear");
        // } else {
        //   lcd.print("void ");
        }
      }
      LCDupdateMain();
    break;


    case 1:  // Set Upper Temperatur
      if (lastframe != menutitle) {
        LCDbuildSetTemp(0);
        lastframe = menutitle;
        menu_entry_time = millis();
      } else if (TimeNow - menu_entry_time > MenuEntryDelay) {
        LCDupdateSetTemp(0);

        lcd.setCursor(10,0);
        if (Button_ok.getState()) { // --> Mainmenu
          priormenu = menutitle;
          menutitle = 3;
          // lcd.print("ok   ");
        } else if (Button_down.getState()) { // --> set lower Temp
          setTemp_Off -= 0.01;
          // lcd.print("down ");
        } else if (Button_up.getState()) { // --> set upper Temp
          setTemp_Off += 0.01;
          // lcd.print("up   ");
        } else if (Button_cancel.getState()) { // --> Mainmenu
          priormenu = menutitle;
          menutitle = 0;
          // lcd.print("clear");
        // } else {
        //   lcd.print("void ");
        }
      }
    break;

    case 2:  // Set Lower Temperature
      if (lastframe != menutitle) {
        LCDbuildSetTemp(1);
        lastframe = menutitle;
        menu_entry_time = millis();
      } else if (TimeNow - menu_entry_time > MenuEntryDelay) {
        LCDupdateSetTemp(1);

        lcd.setCursor(10,0);
        if (Button_ok.getState()) { // --> Mainmenu
          priormenu = menutitle;
          menutitle = 3;
          // lcd.print("ok   ");
        } else if (Button_down.getState()) { // --> set lower Temp
          setTemp_On -= 0.01;
          // lcd.print("down ");
        } else if (Button_up.getState()) { // --> set upper Temp
          setTemp_On += 0.01;
          // lcd.print("up   ");
        } else if (Button_cancel.getState()) { // --> Mainmenu
          priormenu = menutitle;
          menutitle = 0;
          // lcd.print("clear");
        // } else {
        //   lcd.print("void ");
        }
      }
    break;


    case 3:  // confirm
      if (lastframe != menutitle) {
        LCDbuildConfirm();
        lastframe = menutitle;
        menu_entry_time = millis();
      } else if (TimeNow - menu_entry_time > MenuEntryDelay) {
        lcd.setCursor(10,0);
        if (Button_ok.getState()) {  // --> Mainmenu
          Temp_Off = setTemp_Off;
          Temp_On = setTemp_On;
          priormenu = menutitle;
          menutitle = 0;
          // lcd.print("ok   ");
        } else if (Button_down.getState()) { // --> x
          // lcd.print("down ");
        } else if (Button_up.getState()) { // --> x
          // lcd.print("up   ");
        } else if (Button_cancel.getState()) { // --> Mainmenu
          priormenu = menutitle;
          menutitle = 0;
          // lcd.print("clear");
        // } else {
        //   lcd.print("void ");
        }
      }
    break;

    case 4:  // bad temperatur
      controlerstate = 0;
      PowerState = 0;
      if (lastframe != menutitle) {
        lcd.clear();
        lcd.print("E:bad Temperatur");
        lcd.setCursor(0, 1);
        lcd.print("try press reset");
        lastframe = menutitle;
        menu_entry_time = millis();
      }
    break;

    default:
      menutitle = 0;
    break;
  }

  TimeNow = millis();  // update time AFTER buttons, so last event is alsways prior to now.

  lcdBacklight();

  Temp_Off = constrain(Temp_Off, 12, 50);

  if (TimeNow - lastMeasurTime > Messintervall) {
    lastMeasurTime = TimeNow;
    Temperatur = Messung();
  }

  if ((Temperatur < 0) || (Temperatur > 50)) {
    controlerstate = 0;
    PowerState = 0;
    menutitle = 4;
  }

  // Heitzung steuern
  if (controlerstate) {
    digitalWrite(LED_BUILDIN, HIGH);
    if (Temperatur > Temp_Off) {  // Ausschalten
      PowerState = 0;
    } else if (Temperatur < Temp_On) {  // Einschalten
      if (PowerState == 0) { Cyclen++; }
      PowerState = 1;
    }
  } else {
    PowerState = 0;
    digitalWrite(LED_BUILDIN, LOW);
  }

  Heater(PowerState);

  //TimeToString(TimeNow);
}
