#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

#define DHTPIN D2
#define DHTTYPE DHT11
#define RELAY_PIN D4

// ==== Wi-Fi credentials ====
const char* ssid = "pheonixscar";          // your Wi-Fi name
const char* password = "Pheonixscar08@";    // your Wi-Fi password

// ==== MQTT Broker (your laptop IP) ====
const char* mqtt_server = "10.62.15.75"; // <<---- YOUR IPv4 ADDRESS

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

// ==== Variables ====
float temp_c = 0.0;
float humidity = 0.0;
float setpoint_temp = 25.0;
bool relayState = false;
float hysteresis = 2.0;

// ==== Wi-Fi connection ====
void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Wi-Fi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// ==== MQTT callback ====
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) message += (char)payload[i];
  Serial.printf("Message arrived [%s]: %s\n", topic, message.c_str());

  if (String(topic) == "home/thermostat/setpoint") {
    setpoint_temp = message.toFloat();
    Serial.printf("Setpoint updated: %.2f°C\n", setpoint_temp);
  }
  else if (String(topic) == "home/thermostat/control") {
    if (message == "ON")  relayState = true;
    else if (message == "OFF") relayState = false;
    digitalWrite(RELAY_PIN, relayState);
    Serial.printf("Manual control → Relay: %s\n", relayState ? "ON" : "OFF");
  }
}

// ==== MQTT reconnect ====
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("NodeMCU_Client")) {
      Serial.println("connected");
      client.subscribe("home/thermostat/control");
      client.subscribe("home/thermostat/setpoint");
    } else {
      Serial.printf("failed, rc=%d  retry in 5 s\n", client.state());
      delay(5000);
    }
  }
}

// ==== Setup ====
void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

// ==== Main Loop ====
void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  static unsigned long lastMsg = 0;
  if (millis() - lastMsg > 2000) {             // every 2 s
    lastMsg = millis();
    humidity = dht.readHumidity();
    temp_c = dht.readTemperature();

    if (isnan(temp_c) || isnan(humidity)) return;

    // Publish sensor data
    client.publish("home/thermostat/temperature", String(temp_c).c_str());
    client.publish("home/thermostat/humidity", String(humidity).c_str());

    // Automatic control
    if (temp_c > setpoint_temp && !relayState) {
      relayState = true;
      digitalWrite(RELAY_PIN, HIGH);
      client.publish("home/thermostat/relay", "ON");
    } else if (temp_c < setpoint_temp - hysteresis && relayState) {
      relayState = false;
      digitalWrite(RELAY_PIN, LOW);
      client.publish("home/thermostat/relay", "OFF");
    }

    Serial.printf("Temp: %.1f°C | Hum: %.1f%% | Relay: %s\n",
                  temp_c, humidity, relayState ? "ON" : "OFF");
  }
}
