/*
  THERMALINK v1.1 — ESP32 control-station receiver
  OLED: SSD1306 128x64 I2C, SDA=21, SCL=22, address=0x3C
  Active buzzer: GPIO26 (small, low-current, GPIO-compatible only)
  ESP-NOW channel: 1
  ESP32 Arduino core: 3.x receive callback API

  DHT11 readings represent ambient air temperature, not machine surface
  or internal temperature. Thresholds are demonstration values.
*/

#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define CHANNEL 1

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS 0x3C

#define WARNING_TEMP 31.0
#define CRITICAL_TEMP 40.0

#define BUZZER_PIN 26

Adafruit_SSD1306 display(
  SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1
);

bool oledReady = false;

struct __attribute__((packed)) SensorPacket {
  uint32_t sequence;
  float tempC;
  float humidity;
  uint8_t sensorOK;
};

SensorPacket latestPacket = {};

bool packetReceived = false;
uint32_t lastPacketMillis = 0;

portMUX_TYPE dataMux = portMUX_INITIALIZER_UNLOCKED;

void OnDataRecv(
  const esp_now_recv_info_t *info,
  const uint8_t *data,
  int len
) {
  if (len != sizeof(SensorPacket)) {
    return;
  }

  SensorPacket incoming;
  memcpy(&incoming, data, sizeof(incoming));

  portENTER_CRITICAL(&dataMux);
  latestPacket = incoming;
  packetReceived = true;
  lastPacketMillis = millis();
  portEXIT_CRITICAL(&dataMux);
}

void printStationMAC() {
  uint8_t mac[6];

  if (esp_wifi_get_mac(WIFI_IF_STA, mac) == ESP_OK) {
    Serial.printf(
      "ESP32 STA MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
      mac[0], mac[1], mac[2],
      mac[3], mac[4], mac[5]
    );
  } else {
    Serial.println(F("Could not read ESP32 STA MAC."));
  }
}

void updateBuzzer(const char *status) {
  bool alert =
    strcmp(status, "WARNING") == 0 ||
    strcmp(status, "CRITICAL") == 0;

  digitalWrite(BUZZER_PIN, alert ? HIGH : LOW);
}

void drawDashboard() {
  SensorPacket p;
  bool received;
  uint32_t lastReceived;

  portENTER_CRITICAL(&dataMux);
  p = latestPacket;
  received = packetReceived;
  lastReceived = lastPacketMillis;
  portEXIT_CRITICAL(&dataMux);

  bool online =
    received && (millis() - lastReceived <= 10000);

  bool validReading =
    online &&
    p.sensorOK &&
    isfinite(p.tempC) &&
    isfinite(p.humidity) &&
    p.tempC >= -40.0 && p.tempC <= 80.0 &&
    p.humidity >= 0.0 && p.humidity <= 100.0;

  const char *status;

  if (!received) {
    status = "WAITING";
  } else if (!online) {
    status = "NODE OFFLINE";
  } else if (!p.sensorOK) {
    status = "SENSOR ERROR";
  } else if (!validReading) {
    status = "INVALID DATA";
  } else if (p.tempC >= CRITICAL_TEMP) {
    status = "CRITICAL";
  } else if (p.tempC >= WARNING_TEMP) {
    status = "WARNING";
  } else {
    status = "NORMAL";
  }

  updateBuzzer(status);

  Serial.print(F("Link: "));
  Serial.print(online ? F("ONLINE") : F("OFFLINE"));
  Serial.print(F(" | Status: "));
  Serial.println(status);

  if (!oledReady) return;

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.setCursor(0, 0);
  display.println(F("THERMALINK v1.1"));

  display.setCursor(0, 10);
  display.print(F("Link: "));
  display.println(online ? F("ONLINE") : F("OFFLINE"));

  display.setCursor(0, 20);
  display.print(F("Temp: "));

  if (validReading) {
    display.print(p.tempC, 1);
    display.println(F(" C"));
  } else {
    display.println(F("--"));
  }

  display.setCursor(0, 30);
  display.print(F("Humidity: "));

  if (validReading) {
    display.print(p.humidity, 0);
    display.println(F(" %"));
  } else {
    display.println(F("--"));
  }

  display.setCursor(0, 40);
  display.print(F("Status: "));
  display.println(status);

  display.setCursor(0, 50);
  display.print(F("Packet: "));

  if (received) {
    display.println(p.sequence);
  } else {
    display.println(F("--"));
  }

  display.display();
}

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  Serial.println();
  Serial.println(F("THERMALINK v1.1 RECEIVER"));

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  if (esp_wifi_set_channel(
        CHANNEL,
        WIFI_SECOND_CHAN_NONE
      ) != ESP_OK) {
    Serial.println(F("Channel setup failed."));
  }

  printStationMAC();

  if (esp_now_init() != ESP_OK) {
    Serial.println(F("ESP-NOW initialization failed."));
    return;
  }

  if (esp_now_register_recv_cb(OnDataRecv) != ESP_OK) {
    Serial.println(F("Receive callback registration failed."));
    return;
  }

  Serial.println(F("ESP-NOW receiver ready."));

  Wire.begin(21, 22);

  oledReady = display.begin(
    SSD1306_SWITCHCAPVCC,
    OLED_ADDRESS
  );

  if (oledReady) {
    display.clearDisplay();
    display.display();
    Serial.println(F("OLED ready."));
  } else {
    Serial.println(F("OLED not detected; serial monitoring remains available."));
  }
}

void loop() {
  static uint32_t lastDisplayUpdate = 0;

  if (millis() - lastDisplayUpdate >= 500) {
    lastDisplayUpdate = millis();
    drawDashboard();
  }
}
