/*
  TILTGUARD — ESP8266 bus-unit transmitter
  Board: LOLIN NodeMCU V1.0 / ESP-12E
  Sensor: GY-61 (ADXL335), XOUT -> A0 (only after checking A0 voltage limit)

  Sends filtered ADC deviation counts over ESP-NOW channel 1.
  It deliberately does NOT label ADC counts as degrees.
*/

#include <ESP8266WiFi.h>
#include <espnow.h>
#include <user_interface.h>

#define SENSOR_PIN A0

// Replace these bytes with the ESP32 STA MAC printed by receiver.ino.
uint8_t RECEIVER_MAC[6] = {
  CENSORED
};

static const uint32_t PACKET_MAGIC = 0x54494C54;
static const uint8_t ESPNOW_CHANNEL = 1;
static const unsigned long SEND_INTERVAL_MS = 100;
static const float FILTER_ALPHA = 0.25f;

struct TiltPacket {
  uint32_t magic;
  uint32_t sequence;
  float adcDeviation;
};

TiltPacket packet;

float zeroOffset = 0.0f;
float filteredReading = 0.0f;
uint32_t sequenceNumber = 0;
unsigned long lastSend = 0;

void setup() {
  Serial.begin(115200);
  delay(300);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  wifi_set_channel(ESPNOW_CHANNEL);
  delay(100);

  Serial.println();
  Serial.println(F("TILTGUARD ESP8266 transmitter"));
  Serial.print(F("Sender STA MAC: "));
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != 0) {
    Serial.println(F("ERROR: ESP-NOW init failed"));
    while (true) delay(1000);
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

  if (esp_now_add_peer(
        RECEIVER_MAC,
        ESP_NOW_ROLE_SLAVE,
        ESPNOW_CHANNEL,
        NULL,
        0) != 0) {
    Serial.println(F("ERROR: Could not add ESP32 peer"));
    Serial.println(F("Check RECEIVER_MAC in this sketch."));
    while (true) delay(1000);
  }

  Serial.println(F("Keep sensor still and level: calibrating zero..."));

  uint32_t total = 0;
  const int samples = 150;

  for (int i = 0; i < samples; i++) {
    total += analogRead(SENSOR_PIN);
    delay(10);
  }

  zeroOffset = (float)total / samples;
  filteredReading = zeroOffset;

  Serial.print(F("Zero ADC offset: "));
  Serial.println(zeroOffset, 2);
  Serial.println(F("Transmitter ready."));
}

void loop() {
  const int raw = analogRead(SENSOR_PIN);
  filteredReading += FILTER_ALPHA * ((float)raw - filteredReading);

  const float deviation = fabs(filteredReading - zeroOffset);

  if (millis() - lastSend >= SEND_INTERVAL_MS) {
    lastSend = millis();

    packet.magic = PACKET_MAGIC;
    packet.sequence = ++sequenceNumber;
    packet.adcDeviation = deviation;

    const int result = esp_now_send(
      RECEIVER_MAC,
      (uint8_t *)&packet,
      sizeof(packet)
    );

    Serial.print(F("ADC deviation: "));
    Serial.print(deviation, 1);
    Serial.print(F(" | send request: "));
    Serial.println(result == 0 ? F("queued") : F("failed"));
  }

  delay(2);
}
