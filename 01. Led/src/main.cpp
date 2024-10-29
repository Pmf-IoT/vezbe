// upload procedure
// left button - EN  right button - ID0
// 1. press Left Btn and hold it
// 2. press Right Btn and hold it
// 3. release Left Btn
// 4. release Right Btn

// Blink LED

#include <Arduino.h>

#define LED 2

void setup(void) {
  pinMode(LED,OUTPUT); // Set pin mode

  Serial.begin(115200);
  Serial.printf ("Setup finised.\n");
}


void loop(void) {
  delay(300);
  digitalWrite(LED, HIGH);
  
  delay(300);
  digitalWrite(LED, LOW);
}