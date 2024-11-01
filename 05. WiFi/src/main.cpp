// upload procedure
// left button - EN  right button - ID0
// 1. press Left Btn and hold it
// 2. press Right Btn and hold it
// 3. release Left Btn
// 4. release Right Btn

// Povezivanje na WiFi

// https://randomnerdtutorials.com/esp32-useful-wi-fi-functions-arduino/

#include <Arduino.h>

#include <WiFi.h>

#define WIFI_SSID "????"
#define WIFI_PASSWORD "?????"

// MAC addres
uint8_t baseMac[6];
char apName[] = "ada-xxxxxxxxxxxx";
char mac_adr [13];

void setup(void) {
  Serial.begin(115200);

	esp_read_mac(baseMac, ESP_MAC_WIFI_STA); // Get MAC address for WiFi station
  
	sprintf (apName,  "ada-%02X%02X%02X%02X%02X%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
  sprintf (mac_adr, "%02X%02X%02X%02X%02X%02X",  baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);

  WiFi.setHostname(apName);  

  WiFi.mode(WIFI_STA);
  
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }

  Serial.println ();
  Serial.print ("IP adresa: ");
  Serial.println (WiFi.localIP());

  Serial.printf ("\nSetup zavrsen.\n");
}

void loop (void) {
 
}

