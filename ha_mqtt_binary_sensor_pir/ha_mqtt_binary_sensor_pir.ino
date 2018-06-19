/*
   MQTT Binary Sensor - Motion (PIR) for Home-Assistant - NodeMCU (ESP8266)
   https://home-assistant.io/components/binary_sensor.mqtt/

   Libraries :
    - ESP8266 core for Arduino : https://github.com/esp8266/Arduino
    - PubSubClient : https://github.com/knolleary/pubsubclient

   Sources :
    - File > Examples > ES8266WiFi > WiFiClient
    - File > Examples > PubSubClient > mqtt_auth
    - File > Examples > PubSubClient > mqtt_esp8266
    - https://learn.adafruit.com/pir-passive-infrared-proximity-motion-sensor/using-a-pir

   Schematic :
    - https://github.com/mertenats/open-home-automation/blob/master/ha_mqtt_binary_sensor_pir/Schematic.png
    - PIR leg 1 - VCC
    - PIR leg 2 - D1/GPIO5
    - PIR leg 3 - GND
    
   Configuration (HA) : 
     binary_sensor:
      platform: mqtt
      state_topic: 'office/motion/status'
      name: 'Motion'
      sensor_class: motion
      
   TODO :
    - Use the interrupts instead of constinously polling the sensor

   Samuel M. - v1.1 - 08.2016
   If you like this example, please add a star! Thank you!
   https://github.com/mertenats/open-home-automation
   
   
   Modifying the original code to work with the 433mhz doorbell project from TheHookup (?) 
   project needs to sense a pin being pulled high and send a single mqtt message notifying HASS of the flashing LED.
   project uses D0 (GPIO16) on the MCU ESP8266 board for sensing. 
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define MQTT_VERSION MQTT_VERSION_3_1_1

// Wifi: SSID and password
const PROGMEM char* WIFI_SSID = "[Redacted]";
const PROGMEM char* WIFI_PASSWORD = "[Redacted]";

// MQTT: ID, server IP, port, username and password
const PROGMEM char* MQTT_CLIENT_ID = "doorbell";
const PROGMEM char* MQTT_SERVER_IP = "[Redacted]";
const PROGMEM uint16_t MQTT_SERVER_PORT = 1883;
const PROGMEM char* MQTT_USER = "[Redacted]";
const PROGMEM char* MQTT_PASSWORD = "[Redacted]";

// MQTT: topic
const PROGMEM char* MQTT_STATUS_TOPIC = "doorbell";

// default payload
const PROGMEM char* LED_ON = "ON";
const PROGMEM char* LED_OFF = "OFF";

// PIR : D1/GPIO5
const PROGMEM uint8_t PIR_PIN = 5;
uint8_t m_pir_state = LOW; // no motion detected
uint8_t m_pir_value = 0;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

// function called to publish the state of the pir sensor
void publishPirSensorState() {
  if (m_pir_state) {
    client.publish(MQTT_STATUS_TOPIC, LED_OFF, true);
  } else {
    client.publish(MQTT_STATUS_TOPIC, LED_ON, true);
  }
}

// function called when a MQTT message arrived
void callback(char* p_topic, byte* p_payload, unsigned int p_length) {
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("INFO: Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("INFO: connected");
    } else {
      Serial.print("ERROR: failed, rc=");
      Serial.print(client.state());
      Serial.println("DEBUG: try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  // init the serial
  Serial.begin(115200);

  // init the WiFi connection
  Serial.println();
  Serial.println();
  Serial.print("INFO: Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("INFO: WiFi connected");
  Serial.println("INFO: IP address: ");
  Serial.println(WiFi.localIP());

  // init the MQTT connection
  client.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // read the sense pin
  m_pir_value = digitalRead(PIR_PIN);
  if (m_pir_value == HIGH) {
    if (m_pir_state == LOW) {
      // a motion is detected
      Serial.println("INFO:LED on");
      publishPirSensorState();
      m_pir_state = HIGH;
      delay(10000) // added a delay of 10 seconds to let flashing finish. may need to lengthen this after testing. 
    }
  } else {
    if (m_pir_state == HIGH) {
      //publishPirSensorState();  commented ot to remove off MQTT notifications
      Serial.println("INFO:LED off"); //left in to see serial off notifications
      m_pir_state = LOW;
    }
  }
}
