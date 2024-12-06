#include <Arduino.h>

#include <WiFi.h>
extern "C" {
	#include "freertos/FreeRTOS.h"
	#include "freertos/timers.h"
}
#include <AsyncMqttClient.h>

AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;


#define LED_BUILTIN 2

#define WIFI_SSID ""
#define WIFI_PASSWORD ""

#define MQTT_HOST IPAddress (5, 196, 78, 28) // test.mosquitto.org
#define MQTT_PORT 1883                          
#define MQTT_USERNAME ""
#define MQTT_PASSWORD ""

//MQTT topics definitions
const char cc_topic_to_get_temp [] = "/sensor/to/get/temp/";    //  /sensor/to/get/temp/<client MAC adr>  server temp? --> senzor 
const char cc_topic_to_get_led []  = "/sensor/to/get/led/";     //  /sensor/to/get/led/<client MAC adr>   server led? --> senzor

const char cc_topic_to_set_led [] = "/sensor/to/set/led/";      //  /sensor/to/set/led/<client MAC adr>   server led=0/1 --> senzor

const char cc_topic_from_temp [] = "/sensor/from/temp/";        //  /sensor/from/temp/<client MAC adr>    senzor temp=21.5 --> server 
const char cc_topic_form_led []  = "/sensor/from/led/";         //  /sensor/from/led/<client MAC adr>     senzor led=1 --> server

char topic_to_get_temp [100];
char topic_to_get_led [100];
char topic_to_set_led [100];

char topic_from_temp  [100];
char topic_form_led [100];

uint8_t baseMac[6];
char apName[] = "ada-xxxxxxxxxxxx";
char mac_adr [13];

float cur_temp=0.0;
int cur_led=0;


void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void WiFiEvent(WiFiEvent_t event) {
    Serial.printf("[WiFi-event] event: %d\n", event);
    switch(event) {
      case SYSTEM_EVENT_STA_GOT_IP:
          Serial.println("WiFi connected");
          Serial.println("IP address: ");
          Serial.println(WiFi.localIP());
          connectToMqtt();
          break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
          Serial.println("WiFi lost connection");
          xTimerStop(mqttReconnectTimer, 0);  // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
          xTimerStart(wifiReconnectTimer, 0);
          break;
    }
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");

  // setting topics for received
	sprintf (topic_to_get_temp, "%s%s", cc_topic_to_get_temp, mac_adr); 
  uint16_t packetIdSub_temp = mqttClient.subscribe(topic_to_get_temp, 2);
  Serial.printf ("Subscribing %s at QoS 2, packetId: %d \n", topic_to_get_temp, packetIdSub_temp);

	sprintf (topic_to_get_led, "%s%s", cc_topic_to_get_led, mac_adr); 
  uint16_t packetIdSub_get_led = mqttClient.subscribe(topic_to_get_led, 2);
  Serial.printf ("Subscribing %s at QoS 2, packetId: %d \n", topic_to_get_led, packetIdSub_get_led);  

	sprintf (topic_to_set_led, "%s%s", cc_topic_to_set_led, mac_adr); 
  uint16_t packetIdSub_set_led = mqttClient.subscribe(topic_to_set_led, 2);
  Serial.printf ("Subscribing %s at QoS 2, packetId: %d \n", topic_to_set_led, packetIdSub_set_led); 

  // setting topics to send
  sprintf (topic_from_temp, "%s%s", cc_topic_from_temp, mac_adr);
  sprintf (topic_form_led, "%s%s", cc_topic_form_led, mac_adr);  
} // of onMqttConnect

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  char msg [10];
  char resp [10];

  if (strcmp (topic, topic_to_get_temp) == 0) {   // server temp? ---> sensor
    Serial.printf ("mqtt rec=%s\n", topic); 

    cur_temp = 19.5;
    sprintf (resp, "%6.2f", cur_temp); 

    uint16_t packetIdPub = mqttClient.publish (topic_from_temp, 2, true, resp);   // sensor temp=19.5 ---> server

    Serial.printf ("get temp=%s\n", resp);

    return;
  } // of if (strcmp (topic, topic_to_get_temp)

  if (strcmp (topic, topic_to_get_led) == 0) {  // server led? ---> sensor
    Serial.printf ("mqtt rec=%s\n", topic); 

    cur_led = 1;
    sprintf (resp, "%d", cur_led); 

    uint16_t packetIdPub = mqttClient.publish (topic_form_led, 2, true, resp);   // sensor led=0/1 ---> server

    Serial.printf ("get led state=%s\n", resp);

    return;
  } // of if (strcmp (topic, topic_to_get_led)  

  if (strcmp (topic, topic_to_set_led) == 0) {  // server led=0/1 ---> sensor
    for (int i = 0; i < len; i++) 
      msg [i] = (char)payload[i];
    
    msg [len-2] = '\0';

    if (msg[0] == '0') {
      cur_led = 0;
      digitalWrite (LED_BUILTIN, LOW);
      Serial.printf ("led off=%s\n", msg); 
    }  
    else {
      if (msg[0] == '1') {
        cur_led = 1;
        digitalWrite (LED_BUILTIN, HIGH);
        Serial.printf ("led on=%s\n", msg); 
      }
    }

    Serial.printf ("mqtt rec=%s\n", msg); 

    return;
  } // of if (strcmp (topic, topic_to_set_led)    
}

void onMqttPublish(uint16_t packetId) {
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}


void setup() {

  Serial.begin(115200);

  pinMode (LED_BUILTIN, OUTPUT);
  digitalWrite (LED_BUILTIN, LOW);

  // create name 
	esp_read_mac(baseMac, ESP_MAC_WIFI_STA); // Get MAC address for WiFi station
  
	sprintf (apName,  "ada-%02X%02X%02X%02X%02X%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
  sprintf (mac_adr, "%02X%02X%02X%02X%02X%02X",  baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);

  WiFi.setHostname(apName);
  // end of create name

  Serial.printf ("Device name: %s \n", apName);

  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));  

  WiFi.onEvent(WiFiEvent);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCredentials(MQTT_USERNAME, MQTT_PASSWORD);

  connectToWifi();
}

void loop() {
  // put your main code here, to run repeatedly:
}

