#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor
#define DHTTYPE DHT11   // DHT 11

#define dht_dpin 0
DHT dht(dht_dpin, DHTTYPE); 

const char* ssid="RP3";
const char* pass="12345678900"; 
#define AIO_SERVER     "10.42.0.1"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL

int x;
float h;
float t;


WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT);

Adafruit_MQTT_Publish moisture02 = Adafruit_MQTT_Publish(&mqtt,"project/moisture/2");

Adafruit_MQTT_Publish temp = Adafruit_MQTT_Publish(&mqtt,"project/temp");

Adafruit_MQTT_Publish hum = Adafruit_MQTT_Publish(&mqtt,"project/hum");

void MQTT_connect();

void setup() {
  dht.begin();
  Serial.begin(115200);
  Serial.println(); 
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid,pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  
}


void loop() {
  x=analogRead(A0);
  t= dht.readTemperature();
  h= dht.readHumidity();
  
  MQTT_connect();
  Serial.print(F("\nSending myValue "));
  Serial.print("...");
  Serial.println(x);
  if (! moisture02.publish(x)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
  if (! temp.publish(t)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
  if (! hum.publish(h)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  delay(15000);
}


void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
