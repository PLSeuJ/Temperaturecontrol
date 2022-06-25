//-------------------------------------------------------------------------
// Temperatur Regelung 
// Version: V0.5
// created: 29.01.2021
// last edit: 16.04.2021
// author: Jonathan Schumann
// mail: jonathanschumann@gmx.de
//
// for an english version contact the author please.
//
// Das Sketch ist für einen Arduino Nano geschrieben.
// Über ein an Pin 7 angesteuertes Relay wird eine Heitzung betrieben;
// die Ein- und Ausschaltung der Heitzung erfolgt, sobalt die an Pin D2 gemessene Temperatur,
// außerhalb des Temperaturbereichs (Soll +- Delta) liegt.
//
// plan: V0.4: Das Intervall soll über die beiden Knöpfe an Pin 3 und 4 eingestellt werden können
//-------------------------------------------------------------------------
#include <OneWire.h>
#include <LCD_I2C.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 2

// Zuweisung der Anschlüsse
LCD_I2C lcd(0x27); //, 16, 2); //A4 und A5 für SDA und SCL
// LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3,POSITIVE);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const int SensorPin     = 2; // Digital Pin für digitales Eingangssignal
const int UpButtonPin   = 3; // Digital Pin für digitales Eingangssignal
const int DownButtonPin = 4; // Digital Pin für digitales Eingangssignal
const int PowerPin      = 7; // Digital Pin für digitales Ausgangssignal

// Globale Variablen
int PowerState = 0;
int UpButtonState = 0;
int DownButtonState = 0;

float MesswertTemperatur = 0; // Rohwert für Temperatur
float Temperatur = 0; // Temperatur in °C
float SollTemperatur = 38; // Solltemperatur
float Delta = 0,5; // Abweichungstolleranz von Solltemperatur
// nachfolgendes Min und Max löschen
float SollMax = 37; // Eingestellte Maximal Temperatur in °C
float SollMin = SollMax; // Eingestellte Minimal Temperatur in °C

int t = 5*60*1000; // Wartezeit = min × sek/min × msek/sek 
unsigned long Timestamp1;
unsigned long Timestamp2;

//Programm start
void setup()
 {
  Serial.begin(9600); // Serielle Schnittstelle starten für einfacheres testen

  Timestamp1 = millis();
  Timestamp2 = millis();
  
  sensors.begin(); // Temperatursensor konfigurieren
  
  lcd.begin(); // startet den LCD Bidschirm
  lcd.clear(); // Bildschirmausgaben löschen und Curor auf 0,0 setzen
  lcd.backlight(); // Hintergrundbeleuchtung einschalten
  lcd.print("Start");
  lcd.noBlink();

  pinMode(SensorPin, INPUT);
  pinMode(UpButtonPin, INPUT_PULLUP);  
  pinMode(DownButtonPin, INPUT_PULLUP);  
  pinMode(PowerPin, OUTPUT);
  pinMode(PowerPin, LOW);

  lcd.clear(); // Bildschirmausgaben löschen und Curor auf 0,0 setzen
//  lcd.noBacklight();            // Hintergrundbeleuchtung ausschalten 
//  lcd.setCursor(5, 1);          // Set the cursor on the first column and first row.
//  lcd.print("Hello World!");    // Print the string "Hello World!"
}

// Programm =================================================================
void loop()
{
  if(millis() - Timestamp1 >= 60000){
    Temperatur = Messung();
    PowerState = CheckAction();
    pinMode(PowerPin, PowerState);

    Timestamp1 = millis();
  }

// Messung =================================================================
int Messung()
{ 
  // 0. Messung
  sensors.requestTemperatures(); // Temperatur anfragen
  MesswertTemperatur = analogRead(SensorPin); // Temperatur auslesen...
  Temperatur = sensors.getTempCByIndex(0); // ...und in °C speichern

  int i = 0; // Schleifenvariable für Auslesevorgänge
  for (i = 0; i < 5; i++) {
    Serial.print(i);
    Serial.print(" ");
        
    if(Temperatur < 0 or Temperatur > 50){
      sensors.requestTemperatures(); // Temperatur anfragen
  
      MesswertTemperatur = analogRead(SensorPin); // Temperatur auslesen...
      Temperatur = sensors.getTempCByIndex(0); // ...und in °C speichern
  
      Serial.println(Temperatur);
      delay(1000);
    }
    Serial.println();
  }

  Serial.println("Messung Beendet");
  return Temperatur
}
// Check Temperatur =======================================================
int CheckAction(){
  if(Temperatur < 0 or Temperatur > 50){ // Abbruchkriterium wegen falscher Messung
    
    PowerState = 0;
    
    lcd.clear(); // Clear the screen
    lcd.print("press reset");
    lcd.blink(); // Blinke mit dem Cursor
    
    Serial.print("Abbruch");
    
  } else { // Normale Temperaturausgabe

    Serial.println(Temperatur); // Serielle Ausgabe des Temperaturwerts

    // Temperatur am LCD ausgeben
    lcd.clear(); // Clear the screen
    lcd.print(Temperatur); 
    lcd.print(" ");
    lcd.print((char)223);
    lcd.print("C");
  
    if(Temperatur < SollMin){ // Einschalten
      PowerState = 1;    
      } else if (Temperatur > SollMax) { // Ausschalten
      PowerState = 0;
    }    
  }
  return PowerState;
}
