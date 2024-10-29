#include <OneWire.h>
#include <DallasTemperature.h>

const int SENSOR_PIN = 23; // pin na koji je povezan DS18B20 sensor tj. njegov DQ pin

OneWire oneWire(SENSOR_PIN);            // oneWire instanca
DallasTemperature tempSensor(&oneWire); // prosledjujemo oneWire instancu DallasTemperature biblioteci

float tempCelsius;    // temp u Celsius
float tempFahrenheit; // temp u Fahrenheit

void setup()
{
  Serial.begin(115200); 

  tempSensor.begin(); // inicijalizacija sensora
}

void loop()
{
  tempSensor.requestTemperatures();            // salje se cmd za dobijanje temp
  tempCelsius = tempSensor.getTempCByIndex(0); // citamo temp in Celsius
  tempFahrenheit = tempCelsius * 9 / 5 + 32;   // pretvaramo temp in Fahrenheit-e

  Serial.print("Temperature: ");
  Serial.print(tempCelsius);    // print temp u Celsius
  Serial.print("°C");

  Serial.print("  ~  ");        
  Serial.print(tempFahrenheit); // print temp u Fahrenheit
  Serial.println("°F");

  delay(500);
}







