// upload procedure
// left button - EN  right button - ID0
// 1. press Left Btn and hold it
// 2. press Right Btn and hold it
// 3. release Left Btn
// 4. release Right Btn

// Read temp from DS18S20 Temperature chip

#include <Arduino.h>

#include <OneWire.h>

#define SENSOR_PIN 23

OneWire ds(SENSOR_PIN); 

byte addr[8];

void setup(void) {

  Serial.begin(115200);

  ds.reset_search();

  if (!ds.search(addr)) {
    ds.reset_search();
    Serial.print("Sensor nije pronadjen.\n");
    return;
  }  

  Serial.print("R=");

  for( int i = 0; i < 8; i++) {
    Serial.print(addr[i], HEX);
    Serial.print(" ");
  }  

  Serial.printf ("\nSetup zavrsen.\n");
}

int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;

void loop (void) {
  byte present = 0;
  byte data[12];

  if ( OneWire::crc8( addr, 7) != addr[7]){
    Serial.printf("CRC nije validan. \n");
    return;
  }

  if ( addr [0] != 0x28) {
    Serial.printf ("Ovaj senzor pripada DS18S20 familiji.\n");
    return;
  }

  ds.reset();

  ds.select(addr);

  ds.write(0x44,1); // start A/D konverzije

  delay(1000);     // 750ms je dovoljno

  present = ds.reset();

  ds.select(addr);

  ds.write(0xBE);  // citamo rezultat A/D konv

  for (int i = 0; i < 9; i++) { // nama treba 9 bajtova
    data[i] = ds.read();
  }

  LowByte = data[0];

  HighByte = data[1];

  TReading = (HighByte << 8) + LowByte;

  SignBit = TReading & 0x8000; // test most sig bit

  if (SignBit) {                        // ako je negativan
    TReading = (TReading ^ 0xffff) + 1; // 2's comp
  }

  Tc_100 = (TReading*100/2);
  Whole = Tc_100 / 1000 ; // separate off the whole and fractional portions
  Fract = Tc_100 % 1000;

  Serial.printf ("%c%d.%dC \n", SignBit ? '-' : '+', Whole, Fract < 10 ? 0 : Fract);
}

