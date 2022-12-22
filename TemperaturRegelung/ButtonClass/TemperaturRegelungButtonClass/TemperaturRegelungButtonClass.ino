/*-------------------------------------------------------------------------
Temperatur Regelung Menü
Version: V0.5
created: 29.01.2021
last edit: 10.12.2022
author: Jonathan Schumann
mail: jonathanschumann@gmx.de


Hier ist die Testumgebung für die Menü-Programmierung
-------------------------------------------------------------------------
*/
#include <OneWire.h>
#include <LCD_I2C.h>
#include <DallasTemperature.h>

// Versionsnummer
const char VersNr[7] = "V0.8.0";

// Zuweisung der Anschlüsse
const int ONE_WIRE_BUS = 2;
const int SensorPin = 2;  // Digital Pin für digitales Eingangssignal
const int PowerPin = 7;  // Digital Pin für digitales Ausgangssignal
//const int VanClockPin = 9;
//const int VanControlPin = 10;

LCD_I2C lcd(0x27);  // A4 - SDA und A5 - SCL

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensor(&oneWire);

// Globale Variablen
// Logic Variables
bool PowerState = 0;

// Temperatur
float MesswertTemperatur = 0; // Rohwert für Temperatur
float Temperatur = 0; // Temperatur in °C
float Temp_On = 28;  // Einschalttemperatur für Lampe in °C
float Temp_Off = 30;  // Ausschalttemperatur für Lampe in °C
// Temp_Off = Temp_Soll + Temp_Delta;  // <--Wenn Wärmeübergang bekannt kann so der Aufheitzvorgang beschleunigt werden

// Zeit
unsigned long Messintervall = 10 * 1000;  // in Sekunden x 1000ms/sek 
unsigned long TimeNow;
unsigned long TimePrev = -9999;  // force imediate measurenment
unsigned long TimeOfLastInput;  // global variabel to keep trak of last action
const unsigned int DisplayStandBy = 3000;  // 

//misc
char timearray[12]; // here runtime is saved in format dd:hh:mm:ss
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


Button Button_up(4);
Button Button_down(5);
Button Button_ok(6);
Button Button_cancel(3);


void lcdBacklight(void)
/* toggles lcd Backlight */ 
{
  bool lcdBacklightStatus = false;

  if ((TimeNow - TimeOfLastInput > DisplayStandBy)) {
    lcd.noBacklight();
    lcdBacklightStatus = false;
  } else if (!lcdBacklightStatus) {
    lcd.backlight();
    lcdBacklightStatus = true;
  }
}

//Programm start
void setup() {
  Serial.begin(9600); // Serielle Schnittstelle starten für einfacheres testen
  
  sensor.begin(); // Temperatursensor konfigurieren
  
  lcd.begin(); // startet den LCD Bidschirm
  lcd.clear(); // Bildschirmausgaben löschen und Curor auf 0,0 setzen
  lcd.backlight(); // Hintergrundbeleuchtung einschalten
  lcd.print(VersNr);

  pinMode(SensorPin, INPUT);
  pinMode(PowerPin, OUTPUT);
  pinMode(PowerPin, LOW);

  TimeOfLastInput = millis(); // initiallize time

  delay(1000);
  lcd.clear(); // Bildschirmausgaben löschen und Curor auf 0,0 setzen
}



float Messung(void)
/* Misst Temperatur und gibt Temperatur in °C zurück. */
{
  sensor.requestTemperatures(); // Temperatur anfragen
  MesswertTemperatur = analogRead(SensorPin); // Temperatur auslesen...
  return sensor.getTempCByIndex(0); // ...und in °C speichern
}

void Relais(bool PowerState) {
  pinMode(PowerPin, PowerState);
}


void Heater(bool PowerState) {
  Relais(PowerState);
  
  lcd.setCursor(10, 1);
  if (PowerState) {
    lcd.print("on ");
  } else {
    lcd.print("off");
  }
}

// Programm
void loop()
{
  TimeNow = millis();

  lcd.setCursor(10, 0);
  if (Button_cancel.isPressed()) {
    lcd.print("cancel");
  } else if (Button_up.isPressed()) {
    lcd.print("up    ");
    Temp_Off = Temp_Off + 0.1;
  } else if (Button_down.isPressed()) {
    lcd.print("down  ");
    Temp_Off = Temp_Off - 0.1;
  } else if (Button_ok.isPressed()) {
    lcd.print("ok    ");
    PowerState = 1;
  } else {
    lcd.print("void  ");
  }

  lcdBacklight();


  
  Temp_Off = constrain(Temp_Off, 12, 50);
  lcd.setCursor(0,1); // neue Zeile
  lcd.print(Temp_Off); 
  lcd.print(" ");
  lcd.print((char)223);
  lcd.print("C");

  if (TimeNow - TimePrev >= Messintervall) {  // time for a measurnement?
    Temperatur = Messung(); // ...und in °C speichern
    TimePrev = TimeNow;  // update time

    // Temperatur am LCD ausgeben
    lcd.setCursor(0, 0);
    lcd.print(Temperatur); 
    lcd.print(" ");
    lcd.print((char)223);
    lcd.print("C");
  }

  // Heitzung steuern
  if (Temperatur > Temp_Off) { // Ausschalten
    PowerState = 0;
  } else if(Temperatur < Temp_On){ // Einschalten
    if (PowerState == 0) {Cyclen++;}
    PowerState = 1;
  } 

  Heater(PowerState);
}
