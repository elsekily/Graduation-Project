#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

const char* ssid="RP3";
const char* pass="12345678900"; 

#define AIO_SERVER      "10.42.0.1"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL


#define motorpin 0

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT);
Adafruit_MQTT_Subscribe motor=Adafruit_MQTT_Subscribe(&mqtt,"project/motor");

void MQTT_connect();
void setup() {
  pinMode(motorpin,OUTPUT);
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
  mqtt.subscribe(&motor); 
}
uint32_t x=0;

void loop() {
  MQTT_connect();
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000)))
  {
    if(subscription==&motor){
      Serial.print(F("Got onoff: "));
      Serial.println((char *)motor.lastread);
      //Serial.println(atoi(led.lastread));
      
      if (strcmp((char *)motor.lastread, "0")== 0) {
        digitalWrite(motorpin, LOW); 
      }
      if (strcmp((char *)motor.lastread, "1")== 0) {
        digitalWrite(motorpin, HIGH); 
        }
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
}
}
}

void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) 
  { // connect will return 0 for connected
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
