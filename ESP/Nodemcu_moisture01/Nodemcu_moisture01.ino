#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

const char* ssid="RP3";
const char* pass="12345678900"; 
#define AIO_SERVER     "10.42.0.1"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL



WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT);

Adafruit_MQTT_Publish moisture01 = Adafruit_MQTT_Publish(&mqtt,"project/moisture/1");

void MQTT_connect();

void setup() {
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
int x;

void loop() {
  x=analogRead(A0);
  MQTT_connect();
  Serial.print(F("\nSending myValue "));
  Serial.print("...");
  Serial.println(x);
  if (! moisture01.publish(x)) {
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
