#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

const char* ssid = "pheonixscar";
const char* password = "Pheonixscar08";

// 👇 Replace this with your laptop's IP from ipconfig
const char* mqtt_server = "172.17.112.69";  

#define DHTPIN D2
#define DHTTYPE DHT11
#define RELAY_PIN D4

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

float temperature = 0.0, humidity = 0.0;
float setpoint_temp = 25.0;
bool relayState = false;
float hysteresis = 2.0;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi connected");
}

void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  Serial.printf("Message arrived [%s]: %s\n", topic, msg.c_str());

  if (String(topic) == "home/thermostat/setpoint") {
    setpoint_temp = msg.toFloat();
  } else if (String(topic) == "home/thermostat/control") {
    if (msg == "ON") relayState = true;
    else if (msg == "OFF") relayState = false;
    digitalWrite(RELAY_PIN, relayState ? HIGH : LOW);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("NodeMCU_Client")) {
      Serial.println("connected");
      client.subscribe("home/thermostat/setpoint");
      client.subscribe("home/thermostat/control");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5s");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  dht.begin();

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  static unsigned long lastMsg = 0;
  if (millis() - lastMsg > 2000) {
    lastMsg = millis();

    humidity = dht.readHumidity();
    temperature = dht.readTemperature();

    if (isnan(temperature) || isnan(humidity)) return;

    client.publish("home/thermostat/temperature", String(temperature).c_str());
    client.publish("home/thermostat/humidity", String(humidity).c_str());

    // Control logic
    if (temperature > setpoint_temp && !relayState) {
      relayState = true;
      digitalWrite(RELAY_PIN, HIGH);
      client.publish("home/thermostat/relay", "ON");
    } else if (temperature < setpoint_temp - hysteresis && relayState) {
      relayState = false;
      digitalWrite(RELAY_PIN, LOW);
      client.publish("home/thermostat/relay", "OFF");
    }

    Serial.printf("Temp: %.1f°C | Hum: %.1f%% | Relay: %s\n", temperature, humidity, relayState ? "ON" : "OFF");
  }
}
