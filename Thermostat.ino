/******************************************************
 Smart Thermostat (ESP8266 NodeMCU + Blynk)
 Components: NodeMCU ESP8266, DHT22, Relay
 Features: Temperature/Humidity monitoring, Appliance control, Blynk dashboard sync
******************************************************/

#define BLYNK_TEMPLATE_ID "TMPL33sBY_43p"
#define BLYNK_TEMPLATE_NAME "Smart Thermostat"
#define BLYNK_AUTH_TOKEN "MF08YXuT3DU1XBkL_KLcIjldI5w7vaM1"
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>         // ✅ ESP8266 WiFi library
#include <BlynkSimpleEsp8266.h>  // ✅ Blynk library for NodeMCU
#include "DHT.h"

// WiFi credentials
char ssid[] = "Rahul's S24 FE";
char pass[] = "rahul2005";

// Pin and Sensor setup
#define DHTPIN D2
#define DHTTYPE DHT11
#define RELAY_PIN D4     

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

// Virtual Pin assignments
#define VPIN_TEMP V0
#define VPIN_HUMIDITY V1
#define VPIN_SETPOINT V2
#define VPIN_APPLIANCE V3

// Global variables
float temp_c = 0.0;
float humidity = 0.0;
float setpoint_temp = 25.0; // Default setpoint (°C)
int relay_state = HIGH;     // Relay OFF initially (active-LOW)

// Relay control helper
void setRelay(int state) {
  digitalWrite(RELAY_PIN, state);
  relay_state = state;
  Blynk.virtualWrite(VPIN_APPLIANCE, relay_state == LOW ? 1 : 0);
}

// ---- Blynk Interaction ----

// Setpoint from app (slider/numeric on V2)
BLYNK_WRITE(VPIN_SETPOINT) {
  setpoint_temp = param.asFloat();
  Serial.print("New setpoint received: ");
  Serial.println(setpoint_temp);
  // Echo back to Blynk app
  Blynk.virtualWrite(VPIN_SETPOINT, setpoint_temp);
}

// Manual ON/OFF from app (button on V3)
BLYNK_WRITE(VPIN_APPLIANCE) {
  int new_state = param.asInt();
  Serial.print("Manual appliance state: ");
  Serial.println(new_state);
  setRelay(new_state == 1 ? LOW : HIGH);
}

// ---- Periodic Sensor/Control Task ----
void readAndSendSensorData() {
  humidity = dht.readHumidity();
  temp_c = dht.readTemperature();

  if (isnan(humidity) || isnan(temp_c)) {
    Serial.println("Failed to read DHT11!");
    return;
  }

  Blynk.virtualWrite(VPIN_TEMP, temp_c);
  Blynk.virtualWrite(VPIN_HUMIDITY, humidity);
  Serial.printf("Temperature: %.2f°C, Humidity: %.2f%%\n", temp_c, humidity);
  Serial.println("Data sent to Blynk.");

  // Hysteresis control: avoid rapid toggling near setpoint
  if (temp_c > setpoint_temp && relay_state == HIGH) {
    setRelay(LOW); // ON
    Serial.println("Temperature above setpoint → Turning ON appliance.");
  } else if (temp_c < setpoint_temp - 2.0 && relay_state == LOW) {
    setRelay(HIGH); // OFF
    Serial.println("Temperature below setpoint → Turning OFF appliance.");
  }
}

// ---- Setup ----
void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  setRelay(HIGH); // Relay OFF at startup
  dht.begin();

  // Connect to WiFi & Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Confirmation message
  Serial.println("✅ Connected to WiFi and Blynk (Internet available)");

  // Sync setpoint from app on startup
  Blynk.syncVirtual(VPIN_SETPOINT);

  // Read sensor every 2 seconds
  timer.setInterval(2000L, readAndSendSensorData);
}

// ---- Main Loop ----
void loop() {
  Blynk.run();
  timer.run();
}
