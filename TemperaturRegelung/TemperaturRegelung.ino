/*-------------------------------------------------------------------------
Temperatur Regelung 
Version: V1.0.0
created: 29.01.2021
last edit: 06.11.2022
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
const char VersNr[7] = "V0.8.0";

// Zuweisung der Anschlüsse
const int ONE_WIRE_BUS = 2;
const int SensorPin = 2;  // Digital Pin für digitales Eingangssignal
const int UpButtonPin = 4;  // Digital Pin für digitales Eingangssignal
const int DownButtonPin = 5;  // Digital Pin für digitales Eingangssignal
const int OkButtonPin = 6;
const int CancelButtonPin = 3;  // 3 ist ein interrupt-pin
const int PowerPin = 7;  // Digital Pin für digitales Ausgangssignal
//const int VanClockPin = 9;
//const int VanControlPin = 10;

LCD_I2C lcd(0x27);  // A4 - SDA und A5 - SCL

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensor(&oneWire);


// Globale Variablen
// Logic Variables
bool PowerState = 0;

bool UpButtonState = 0;
bool DownButtonState = 0;
bool OkButtonState = 0;
bool CancelButtonState = 0;

// Temperatur
float MesswertTemperatur = 0; // Rohwert für Temperatur
float Temperatur = 0; // Temperatur in °C
float Temp_On = 28;  // Einschalttemperatur für Lampe in °C
float Temp_Off = 30;  // Ausschalttemperatur für Lampe in °C
// Temp_Off = Temp_Soll + Temp_Delta;  // <--Wenn Wärmeübergang bekannt kann so der Aufheitzvorgang beschleunigt werden

// Zeit
unsigned long Messintervall = 2 * 1000;  // in Sekunden x 1000ms/sek 
unsigned long TimeNow;
unsigned long TimePrev;


//misc
char timearray[12]; // here runtime is saved in format dd:hh:mm:ss
unsigned long Cyclen = 0;  // Anzahl der Einschaltvorgänge

//Programm start
void setup()
{
  lcd.begin();  // startet den LCD Bidschirm
  lcd.clear();  // Bildschirmausgaben löschen und Curor auf 0,0 setzen
  lcd.backlight();  // Hintergrundbeleuchtung einschalten
  lcd.print(VersNr);
  lcd.noBlink();

  sensor.begin();  // Temperatursensor konfigurieren

  pinMode(SensorPin, INPUT);
  // Buttons
  pinMode(UpButtonPin, INPUT_PULLUP);
  pinMode(DownButtonPin, INPUT_PULLUP);
  pinMode(OkButtonPin, INPUT_PULLUP);
  pinMode(DownButtonPin, INPUT_PULLUP);
  
  pinMode(PowerPin, OUTPUT);
  pinMode(PowerPin, LOW);

  delay(1000);

  TimeNow = millis();
  TimePrev = TimeNow;

  lcd.clear();  // Bildschirmausgaben löschen und Curor auf 0,0 setzen
}

/*
void GetRunTime(Timenow=millis(), timearray)
// return string of Timenow in format dd:hh:mm:ss 
{
  unsigned long currentMillis = millis();
  unsigned long seconds = currentMillis / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  unsigned long days = hours / 24;
  currentMillis %= 1000;
  seconds %= 60;
  minutes %= 60;
  hours %= 24;

  //{days, hours, minutes, seconds} 
}
*/


float Messung(void)
/* Misst Temperatur und gibt Temperatur in °C zurück. */
{
  sensor.requestTemperatures(); // Temperatur anfragen
  MesswertTemperatur = analogRead(SensorPin); // Temperatur auslesen...
  return sensor.getTempCByIndex(0); // ...und in °C speichern
}


// Programm
void loop(){
  TimeNow = millis();

  // < check buttons here >

  if (TimeNow - TimePrev >= Messintervall) {  // time for a measurnement?
    Temperatur = Messung();
    lcd.clear();
    lcd.print(Temperatur);

    if (Temperatur < Temp_On) {  // assert Temperature
      PowerState = 1;
    } else if (Temperatur > Temp_Off) {
      PowerState = 0;
    }

    pinMode(PowerPin, PowerState);  // toggle heater

    TimePrev = TimeNow;  // update time
  }
}
