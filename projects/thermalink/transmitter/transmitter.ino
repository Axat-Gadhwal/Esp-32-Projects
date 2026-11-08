/*
  THERMALINK v1.1 — ESP8266 machine-side transmitter
  Board: LOLIN NodeMCU V1.0 / ESP-12E
  Sensor: DHT11, DATA -> D2 (GPIO4)
  Wireless: ESP-NOW channel 1

  Sends ambient temperature and humidity to an ESP32 receiver.
*/

#include <ESP8266WiFi.h>
#include <espnow.h>
#include <user_interface.h>
#include <DHT.h>

#define DHT_PIN D2
#define DHT_TYPE DHT11

#define CHANNEL 1
#define SEND_INTERVAL 3000

// ESP32 receiver STA MAC address from the working build.
const char *RECEIVER_MAC = "70:4B:CA:17:33:98";

DHT dht(DHT_PIN, DHT_TYPE);

struct __attribute__((packed)) SensorPacket {
  uint32_t sequence;
  float tempC;
  float humidity;
  uint8_t sensorOK;
};

uint8_t receiverMac[6];
uint32_t packetCounter = 0;
uint32_t lastSend = 0;

bool parseMAC(const char *mac, uint8_t *output) {
  unsigned int values[6];

  if (sscanf(
        mac, "%x:%x:%x:%x:%x:%x",
        &values[0], &values[1], &values[2],
        &values[3], &values[4], &values[5]
      ) != 6) {
    return false;
  }

  for (int i = 0; i < 6; i++) {
    if (values[i] > 255) return false;
    output[i] = (uint8_t)values[i];
  }

  return true;
}

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println();
  Serial.println(F("THERMALINK v1.1 TRANSMITTER"));

  dht.begin();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  wifi_set_channel(CHANNEL);

  Serial.print(F("ESP8266 STA MAC: "));
  Serial.println(WiFi.macAddress());

  if (!parseMAC(RECEIVER_MAC, receiverMac)) {
    Serial.println(F("ERROR: Invalid receiver MAC address."));
    while (true) delay(1000);
  }

  if (esp_now_init() != 0) {
    Serial.println(F("ERROR: ESP-NOW initialization failed."));
    while (true) delay(1000);
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

  if (esp_now_add_peer(
        receiverMac,
        ESP_NOW_ROLE_SLAVE,
        CHANNEL,
        NULL,
        0
      ) != 0) {
    Serial.println(F("ERROR: Could not add ESP32 receiver peer."));
    while (true) delay(1000);
  }

  Serial.println(F("Wireless transmitter ready."));
}

void loop() {
  if (millis() - lastSend >= SEND_INTERVAL) {
    lastSend = millis();

    SensorPacket packet = {};

    packet.sequence = ++packetCounter;

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
      packet.sensorOK = 0;
      packet.tempC = 0;
      packet.humidity = 0;
      Serial.println(F("DHT11 reading error."));
    } else {
      packet.sensorOK = 1;
      packet.tempC = temperature;
      packet.humidity = humidity;

      Serial.print(F("Temperature: "));
      Serial.print(temperature);
      Serial.print(F(" C | Humidity: "));
      Serial.print(humidity);
      Serial.println(F(" %"));
    }

    int result = esp_now_send(
      receiverMac,
      (uint8_t *)&packet,
      sizeof(packet)
    );

    Serial.println(
      result == 0
        ? F("Packet queued for transmission.")
        : F("Packet transmission error.")
    );
  }
}
