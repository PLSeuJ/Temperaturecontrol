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
// Der Sketch ist für einen Arduino Nano geschrieben.
// Über ein an Pin 7 angesteuertes Relay wird eine Heitzung betrieben;
// die Ein- und Ausschaltung der Heitzung erfolgt, sobalt die an Pin D2 gemessene Temperatur,
// außerhalb des Temperaturbereichs (Soll +- Delta) liegt.
//
// plan: V0.6: Das Intervall soll über die beiden Knöpfe an Pin 3 und 4 eingestellt werden können
//-------------------------------------------------------------------------
#include <OneWire.h>
#include <LCD_I2C.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 2

// Zuweisung der Anschlüsse
LCD_I2C lcd(0x27); A4 und A5 für SDA und SCL
// LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3,POSITIVE);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const int SensorPin     = 2; // Digital Pin für digitales Eingangssignal
// chnage following button to 
const int PowerPin      = 7; // Digital Pin für digitales Ausgangssignal
//const int OkButtonPin  = 6; // Digital Pin for confirmation of selcted item

// Globale Variablen
int PowerState = 0;
int UpButtonState = 0;
int DownButtonState = 0;

float MesswertTemperatur = 0; // Rohwert für Temperatur
float Temperatur = 0; // Temperatur in °C
float SollTemperatur = 37; // Solltemperatur
float Delta = 0.5; // Abweichungstolleranz von Solltemperatur
// nachfolgendes Min und Max löschen
// float SollMax = 37; // Eingestellte Maximal Temperatur in °C
// float SollMin = SollMax; // Eingestellte Minimal Temperatur in °C

float dtLampe = 60000; // Ruhezeit Lampe = min × sek/min × msek/sek  


class Button {
  private:
    byte pin;
    byte state;
    byte lastReading;
    unsigned long lastDebounceTime = 0;
    unsigned long debounceDelay = 50;
  public:
    Button(byte pin) {
      this->pin = pin;
      lastReading = LOW;
      init();
    }
    void init() {
      pinMode(pin, INPUT);
      update();
    }
    void update() {
      // You can handle the debounce of the button directly
      // in the class, so you don't have to think about it
      // elsewhere in your code
      byte newReading = digitalRead(pin);
      
      if (newReading != lastReading) {
        lastDebounceTime = millis();
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
      return (getState() == HIGH);
    }
}; // don't forget the semicolon at the end of the class

//Programm start
void setup()
 {
  Serial.begin(9600); // Serielle Schnittstelle starten für einfacheres testen
  
  sensors.begin(); // Temperatursensor konfigurieren
  
  lcd.begin(); // startet den LCD Bidschirm
  lcd.clear(); // Bildschirmausgaben löschen und Curor auf 0,0 setzen
  lcd.backlight(); // Hintergrundbeleuchtung einschalten
  lcd.print("Start");
  lcd.noBlink();

  Button UpButton(3);
  Button DownButton(4);
  // Button OkButton();
  // Button CancelButton();

  pinMode(SensorPin, INPUT);
  pinMode(PowerPin, OUTPUT);
  pinMode(PowerPin, LOW);

  lcd.clear(); // Bildschirmausgaben löschen und Curor auf 0,0 setzen
}


//Test at Startup
void test()
{
  // Serial.print("Testing")
  
//
}


// Programm
void loop()
{
  // 0. Messung
  sensors.requestTemperatures(); // Temperatur anfragen
  MesswertTemperatur = analogRead(SensorPin); // Temperatur auslesen...
  Temperatur = sensors.getTempCByIndex(0); // ...und in °C speichern

  int i = 0; // Schleifenvariable für Auslesevorgänge
  for (i = 0; i < 5; i++) { // Prüfe Plasibilität der Temperatur fünf Mal
    if(Temperatur < 0 or Temperatur > 50){ // Prüfe Plasibilität der Temperatur  
      sensors.requestTemperatures(); // Temperatur anfragen
      MesswertTemperatur = analogRead(SensorPin); // Temperatur auslesen...
      Temperatur = sensors.getTempCByIndex(0); // ...und in °C speichern
  
      delay(100);
    } else { // Prüfung vorzeitig erfolgreich beenden
      i = 5; // for-Schleife beenden, da Temperatur plausibel
    }
    Serial.println(i);
  }

  if(Temperatur < 0 or Temperatur > 50){ // Abbruchkriterium wegen falscher Messung
    pinMode(PowerPin, LOW);
    lcd.clear(); // Clear the screen
    lcd.print("press reset");
    lcd.blink(); // Blinke mit dem Cursor
    
  } else { // Normale Temperaturausgabe
    // Temperatur am LCD ausgeben
    lcd.clear(); // Clear the screen
    lcd.print(Temperatur); 
    lcd.print(" ");
    lcd.print((char)223);
    lcd.print("C");

    lcd.setCursor(0,1); // neue Zeile
    lcd.print(SollTemperatur); 
    lcd.print(" ");
    lcd.print((char)223);
    lcd.print("C");
  
    if(Temperatur < SollTemperatur - Delta){ // Einschalten
      pinMode(PowerPin, HIGH);
    } else if (Temperatur > SollTemperatur + Delta) { // Ausschalten
      pinMode(PowerPin, LOW);
    }  
  }
  delay(1000);
}
