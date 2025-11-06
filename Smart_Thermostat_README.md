# Smart Thermostat Using ESP8266 and Blynk

## 1. Overview
The Smart Thermostat project provides automated temperature monitoring and control using an ESP8266 microcontroller, a DHT11 sensor, and the Blynk IoT platform. It enables real-time temperature and humidity tracking, automatic fan or heater activation, and remote access via smartphone or web dashboard.

## 2. Features
- Real-time temperature and humidity monitoring  
- Remote control via Blynk Cloud/App  
- Automatic activation of cooling/heating based on threshold  
- Wi-Fi-enabled communication (ESP8266)  
- Mobile dashboard visualization and control  
- Alert notification for abnormal temperature  

## 3. Hardware Components
| Component | Description |
|------------|--------------|
| ESP8266 (NodeMCU) | Wi-Fi microcontroller for IoT communication |
| DHT11 Sensor | Measures temperature and humidity |
| Relay Module | Controls external devices (fan/heater) |
| Power Supply (5V) | Powers the ESP8266 and peripherals |
| Smartphone / PC | For Blynk control interface |

## 4. Software Requirements
- Arduino IDE (version ≥ 1.8.13)  
- Blynk IoT App (available on Android/iOS)  
- Blynk Library (v1.2.0 or newer)  
- ESP8266 Board Package  
- DHT Sensor Library by Adafruit  

## 5. Installation Steps

### Step 1: Arduino IDE Setup
1. Install Arduino IDE from https://www.arduino.cc/en/software  
2. Open File → Preferences and paste this in Additional Board Manager URLs:  
   https://arduino.esp8266.com/stable/package_esp8266com_index.json
3. Go to Tools → Board → Boards Manager, search for ESP8266, and click Install.  

### Step 2: Install Required Libraries
Go to Sketch → Include Library → Manage Libraries..., then install:  
- Blynk by Volodymyr Shymanskyy  
- DHT sensor library by Adafruit  
- Adafruit Unified Sensor  

### Step 3: Hardware Connections
| Component | ESP8266 Pin | Description |
|------------|--------------|-------------|
| DHT11 VCC | 3V3 | Power supply |
| DHT11 GND | GND | Ground |
| DHT11 DATA | D4 | Signal pin |
| Relay IN | D2 | Control signal |
| Relay VCC | VIN | 5V input |
| Relay GND | GND | Common ground |

### Step 4: Configure Blynk
1. Open Blynk App → Create a new project.  
2. Select Device: NodeMCU (ESP8266) and Connection Type: Wi-Fi.  
3. Copy the Auth Token received via email.  
4. Add widgets for Value Display, Switch Button, and Gauge.

## 6. Configuration Instructions
1. Open Thermostat.ino.  
2. Modify the following lines:
   ```cpp
   char auth[] = "YourAuthToken";
   char ssid[] = "YourWiFiSSID";
   char pass[] = "YourWiFiPassword";
   ```
3. Select Board: “NodeMCU 1.0 (ESP-12E Module)”  
4. Select COM Port → Upload the code.

## 7. Usage Guidelines
1. Power on ESP8266.  
2. Wait for “WiFi connected” and “Connected to Blynk.”  
3. Observe temperature/humidity on Blynk App.  
4. Relay activates automatically or manually via switch.  

## 8. Troubleshooting
| Issue | Cause | Solution |
|-------|--------|-----------|
| “PubSubClient.h: No such file” | Missing library | Install PubSubClient |
| Wi-Fi not connecting | Wrong SSID/Password | Recheck credentials |
| Blynk not updating | Invalid Auth Token | Copy correct token |
| Relay not switching | Wrong pin/power issue | Check connections |
| Temperature stuck | DHT11 issue | Check sensor wiring |

## 9. Expected Output
- Serial Monitor: Displays temperature, humidity, and relay status.  
- Blynk Dashboard: Shows live readings and control.

## 10. References
1. Blynk IoT Documentation: https://docs.blynk.io  
2. ESP8266 Arduino Core: https://arduino-esp8266.readthedocs.io  
3. Adafruit DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library  
4. PubSubClient Library: https://github.com/knolleary/pubsubclient

## 11. Author and Credits
**Project Title:** Smart Thermostat Using ESP8266 and Blynk  
**Developed by:** Rohith Kumar and team.
**Institution:** VIT University  
**Date:** November 2025  
**Version:** 1.0  
