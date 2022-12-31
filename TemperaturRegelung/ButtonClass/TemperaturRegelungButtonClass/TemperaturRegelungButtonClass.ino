/*-------------------------------------------------------------------------
Temperatur Regelung Menü
Version: V0.5
created: 29.01.2021
last edit: 23.12.2022
author: Jonathan Schumann
mail: jonathanschumann@gmx.de


Hier ist die Testumgebung für die Menü-Programmierung
-------------------------------------------------------------------------
*/
#include <OneWire.h>
#include <LCD_I2C.h>
#include <DallasTemperature.h>

// Versionsnummer
const char VersNr[8] = "V0.8.11";

// Zuweisung der Anschlüsse
const int ONE_WIRE_BUS = 2;
const int SensorPin = 2;  // Digital Pin für digitales Eingangssignal
const int PowerPin = 7;   // Digital Pin für digitales Ausgangssignal
//const int VanClockPin = 9;
//const int VanControlPin = 10;

LCD_I2C lcd(0x27);  // A4 - SDA und A5 - SCL

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensor(&oneWire);

// Globale Variablen
// Logic Variables
bool controlerstate = 0;  // ON/OFF toggle
bool PowerState = 0;  // relais power on/off internal variable

// Temperatur
float MesswertTemperatur = 0;  // Rohwert für Temperatur
float Temperatur = 0;          // Temperatur in °C
float Temp_On = 28;            // Einschalttemperatur für Lampe in °C
float Temp_Off = 30;           // Ausschalttemperatur für Lampe in °C
float setTemp;
// Temp_Off = Temp_Soll + Temp_Delta;  // <--Wenn Wärmeübergang bekannt kann so der Aufheitzvorgang beschleunigt werden

// Zeit
unsigned long Messintervall = 10 * 1000;  // in Sekunden x 1000ms/sek
unsigned long TimeNow;
unsigned long TimePrev = -9999;             // force imediate measurenment
unsigned long TimeOfLastInput;              // global variabel to keep trak of last action
const unsigned int DisplayStandBy = 30000;  //

//misc
char timearray[12];        // here runtime is saved in format dd:hh:mm:ss
unsigned long Cyclen = 0;  // Anzahl der Einschaltvorgänge

//menu related variables
int lastframe = 999;
int menutitle = 0;
int priormenu = 999;



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


Button Button_up(4);
Button Button_down(5);
Button Button_ok(6);
Button Button_cancel(3);


void lcdBacklight(void)
/* toggles LCD Backlight */
{
  if (TimeNow - TimeOfLastInput >= DisplayStandBy) {
    lcd.noBacklight();
  } else {
    lcd.backlight();
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

  if (prevState != PowerState) {
    prevState = PowerState;
    lcd.setCursor(8, 1);
    if (PowerState) {
      lcd.print("^");
    } else {
      lcd.print("v");
    }
  }

  if (abs(Temperatur - prevTemp) >= 0.1) {
    prevTemp = Temperatur;
    lcd.print(" ");
    lcd.print(Temperatur, 1);
  }
}


void LCDbuildConfirm() {
  lcd.clear();
  lcd.print("Confirm page");
}


void LCDbuildSetTemp(float T1, float T2, int select) {
  lcd.clear();
  lcd.print(" ");
  lcd.print(Temp_Off, 1);
  lcd.print((char)223);
  lcd.print("C");

  lcd.setCursor(1, 1);
  lcd.print(Temp_On, 1);
  lcd.print((char)223);
  lcd.print("C ");

  lcd.setCursor(0, select);
  lcd.print(">");
}


void LCDbuildSetLower() {
  lcd.clear();
  lcd.print("Set Temp lower");
}

// Programm ---------------------------------------------------
void loop() {


  switch (menutitle) {
    case 0:  // mainmenu
      if (lastframe != menutitle) {
        LCDbuildMain();
        lastframe = menutitle;
      }
      LCDupdateMain();
      lcd.setCursor(10,0);

    if (Button_ok.getState()) { // --> Mainmenu
      controlerstate = true;
      lcd.print("ok   ");
    } else if (Button_down.getState()) { // --> set lower Temp
      menutitle = 2;
      setTemp = Temp_On;
      lcd.print("down ");
    } else if (Button_up.getState()) { // --> set upper Temp
      menutitle = 1;
      setTemp = Temp_Off;
      lcd.print("up   ");
    } else if (Button_cancel.getState()) { // --> Mainmenu
      controlerstate = false;
      lcd.print("clear");
    } else {
      lcd.print("void ");
    }
    break;


    case 1:  // Set Upper Temperatur
      LCDbuildSetTemp(setTemp, Temp_Off, 1);

      lcd.setCursor(10,0);
      if (Button_ok.getState()) { // --> Mainmenu
        lastframe = menutitle;
        menutitle = 3;
        lcd.print("ok   ");
      } else if (Button_down.getState()) { // --> set lower Temp
        setTemp -= 0.1;
        lcd.print("down ");
      } else if (Button_up.getState()) { // --> set upper Temp
        menutitle = 1;
        setTemp += 0.1;
        lcd.print("up   ");
      } else if (Button_cancel.getState()) { // --> Mainmenu
        lastframe = menutitle;
        menutitle = 0;
        lcd.print("clear");
      } else {
        lcd.print("void ");
      }
      break;

    // case 2:  // Set Lower Temperature
    //   LCDbuildSetLower();
    //   lastframe = menutitle;

    //   if (Button_ok.getState()) menutitle = 3;  // --> confirm
    //   if (Button_down.getState())
    //     ;  // --> x
    //   if (Button_up.getState())
    //     ;                                           // --> x
    //   if (Button_cancel.getState()) menutitle = 0;  // --> Mainmenu

    // case 3:  // confirm
    //   LCDbuildConfirm();
    //   lastframe = menutitle;

    //   if (Button_ok.getState()) menutitle = 0;  // --> Mainmenu
    //   if (Button_down.getState())
    //     ;  // --> x
    //   if (Button_up.getState())
    //     ;                                           // --> x
    //   if (Button_cancel.getState()) menutitle = 0;  // --> Mainmenu
    default:
      menutitle = 0;
  }

  TimeNow = millis();  // update time AFTER buttons, so last event is alsways prior to now.

  lcdBacklight();

  Temp_Off = constrain(Temp_Off, 12, 50);

  Temperatur = 23; // Messung();

  // Heitzung steuern
  if (Temperatur > Temp_Off) {  // Ausschalten
    PowerState = 0;
  } else if (Temperatur < Temp_On) {  // Einschalten
    if (PowerState == 0) { Cyclen++; }
    PowerState = 1;
  }

  Heater(PowerState);

  //TimeToString(TimeNow);
}
