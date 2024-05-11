#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <TM1637.h>  // Grove 4-Digit Display

#define REPORTING_PERIOD_MS     1000 

PulseOximeter pox; // Instanz des Objektes PulseOximeter

uint32_t tsLastReport = 0;
int A[15]; // A[0] wird nicht benutzt, weil der Index auch als Divisor dient
int count = 1;
int hrav = 0;
int i = 0;
int spo2 = 0;
int hr = 0;

const int CLK = 2; 
const int DIO = 3;

TM1637 tm1637(CLK, DIO);

void onBeatDetected() {
  Serial.print("♥︎ ");
}

void arrayclear(){
  for (i=1; i==15; i++){
    A[i]=0;
  }
}

void setup() {

    Serial.begin(115200);

    Serial.print("PulseoOxymeter starten ...");

    if (!pox.begin()) {
        Serial.println("Fehler");
        for(;;);
    } else {
        Serial.println("Initialisierung erfolgreich");
    }
    pox.setIRLedCurrent(MAX30100_LED_CURR_27_1MA); // Einstellung für Pachydermen
    pox.setOnBeatDetectedCallback(onBeatDetected);
    
  tm1637.init();
  tm1637.set(BRIGHT_TYPICAL);

  arrayclear(); //Ergebnisarray löschen
}

void loop() {
  
    pox.update(); tm1637.point(POINT_ON);
    
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
      hr = pox.getHeartRate();

      if (count<15){
        
        A[count] = hr;
        hrav = 0;
        for (int i=1; i<count+1; i++ ) {
          hrav = hrav+A[i];
        }
        hrav = hrav/count;
        count++;
      } 
      else {
          hrav = 0;
          
          A[15] = hr; 
          for (int i=1; i<16; i++) {
            hrav = hrav+A[i];
          }
          hrav = hrav/45; // Umrechnungsfaktor
          for (int i=1; i<15; i++) {
            A[i] = A[i+1];
          }
        }

      Serial.print("ø Heart rate:");
      Serial.print(hrav);
      Serial.print("bpm / SpO2:");
      spo2=int(pox.getSpO2());
      Serial.print(spo2);
      Serial.println("%");

      if (spo2 == 0){
          hrav = 0;
          arrayclear(); count = 1;
          tm1637.clearDisplay();
      }
      if (spo2 == 100){
          spo2 = 99;
      }
      if (count ==15) {
        tm1637.display(0, (hrav / 10) % 10);
        tm1637.display(1, hrav % 10);
      }
      tm1637.display(2, (spo2 / 10) % 10);
      tm1637.display(3, spo2 % 10);
      
      tsLastReport = millis();
    }

}