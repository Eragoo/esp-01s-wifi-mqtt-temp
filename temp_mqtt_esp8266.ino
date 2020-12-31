
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT11

const char* ssid = "";
const char* password = "";

const char* mqtt_server = "";
const char* mqttUsername = "";
const char* mqttPassword = "";
const char* mqttClientName = "";
const char* tempTopic = "";

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];


void setup() {
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while(!Serial) { }
  
  
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  client.setServer(mqtt_server, 1883);

   if (!client.connected()) {
    // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqttClientName, mqttUsername, mqttPassword)) {
      Serial.println("Connected to MQTT");
      client.subscribe(tempTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
  }
  client.loop();
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C ");

  const String str = String(t);
  byte resp[str.length()];
  str.getBytes(resp, str.length());
  
  client.beginPublish(tempTopic, str.length(), false);
  client.write(resp, str.length());
  client.endPublish();
  Serial.printf("Published");  

  ESP.deepSleep(30e6);
}

void loop() {
}
