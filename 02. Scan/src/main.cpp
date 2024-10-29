// upload procedure
// left button - EN  right button - ID0
// 1. press Left Btn and hold it
// 2. press Right Btn and hold it
// 3. release Left Btn
// 4. release Right Btn

// Read address of DS18S20 Temperature chip

#include <Arduino.h>

#include <OneWire.h>

#define SENSOR_PIN 23

OneWire ds(SENSOR_PIN); 

void setup(void) {

  Serial.begin(115200);

  Serial.printf ("Setup zavrsen.\n");
}


void loop(void) {

  byte i;

  byte present = 0;

  byte data[12];

  byte addr[8];

  ds.reset_search();

  if ( !ds.search(addr)) {

    Serial.print("Nema vise mogucih adresa.\n");
    ds.reset_search();
    return;
  }

  Serial.print("R=");

  for( i = 0; i < 8; i++) {
    Serial.print(addr[i], HEX);
   Serial.print(" ");
  }


  if ( OneWire::crc8( addr, 7) != addr[7]) {
    Serial.print("CRC nije validan!\n");
    return;
  }

  if ( addr[0] == 0x10) {
    Serial.print("Ovaj senzor pripada DS18S20 familiji.\n");
  }
  else {
    if ( addr[0] == 0x28) {
      Serial.print("Ovaj senzor pripada DS18B20 familiji.\n");
    }
    else {
      Serial.print("Ne moze se prepoznati kojoj familija senzora.: 0x");
      Serial.println(addr[0],HEX);
      return;
    }
  }

  ds.reset();  
  ds.select(addr);
  ds.write(0x44,1); // start A/D konverzije

  delay(1000);      // 750ms je dovoljno

  present = ds.reset();

  ds.select(addr); 

  ds.write(0xBE);   // citamo rezultat A/D konv

  Serial.print("P=");
  Serial.print(present,HEX);
  Serial.print(" ");

  for ( i = 0; i < 9; i++) {    // nama treba 9 bajtova
    data[i] = ds.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }

  Serial.print(" CRC=");

  Serial.print (OneWire::crc8( data, 8), HEX);

  Serial.println();
}