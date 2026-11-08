/*
  TILTGUARD — ESP32 warning-centre receiver
  OLED: SSD1306 128x64 I2C, SDA=21, SCL=22
  Optional active buzzer: GPIO 26 (low-current GPIO-compatible only)

  ESP32 Arduino core 3.x receive callback API.
  Receives ADC deviation counts, not calibrated degrees.
*/

#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <esp_mac.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_ADDRESS 0x3C
#define BUZZER_PIN 26

static const uint32_t PACKET_MAGIC = 0x54494C54;
static const uint8_t ESPNOW_CHANNEL = 1;
static const unsigned long LINK_TIMEOUT_MS = 1500;

// Demonstration thresholds in ADC counts, not degrees.
static const float ALARM_ON_COUNTS = 5.0f;
static const float ALARM_OFF_COUNTS = 3.0f;
static const float WARNING_COUNTS = 15.0f;
static const float CRITICAL_COUNTS = 25.0f;

struct TiltPacket {
  uint32_t magic;
  uint32_t sequence;
  float adcDeviation;
};

Adafruit_SSD1306 display(128, 64, &Wire, -1);

portMUX_TYPE packetMux = portMUX_INITIALIZER_UNLOCKED;
volatile float latestDeviation = 0.0f;
volatile unsigned long lastPacketMs = 0;
volatile bool packetReceived = false;

bool alarmActive = false;
uint32_t lastSequence = 0;

void onDataReceived(
  const esp_now_recv_info_t *info,
  const uint8_t *data,
  int length
) {
  if (length != sizeof(TiltPacket)) return;

  TiltPacket incoming;
  memcpy(&incoming, data, sizeof(incoming));

  if (incoming.magic != PACKET_MAGIC) return;

  portENTER_CRITICAL(&packetMux);
  latestDeviation = incoming.adcDeviation;
  lastPacketMs = millis();
  packetReceived = true;
  portEXIT_CRITICAL(&packetMux);
}

void drawStatus(
  const char *status,
  float deviation,
  bool connected,
  bool buzzerOn
) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(0, 3);
  display.println(status);

  display.setTextSize(1);
  display.setCursor(0, 34);
  display.print(F("ADC deviation: "));
  display.print(deviation, 1);
  display.println(F(" ct"));

  display.setCursor(0, 47);
  display.print(F("LINK: "));
  display.print(connected ? F("OK") : F("LOST"));

  display.setCursor(0, 57);
  display.print(F("ALARM: "));
  display.print(buzzerOn ? F("ON") : F("OFF"));

  display.display();
}

void setup() {
  Serial.begin(115200);
  delay(300);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  Wire.begin(21, 22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("ERROR: SSD1306 not found. Check wiring/address."));
    while (true) delay(1000);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(0, 10);
  display.println(F("TILTGUARD"));
  display.setTextSize(1);
  display.println(F("Starting receiver..."));
  display.display();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(200);

  uint8_t mac[6] = {0};
  esp_read_mac(mac, ESP_MAC_WIFI_STA);

  Serial.print(F("ESP32 STA MAC: "));
  Serial.printf(
    "%02X:%02X:%02X:%02X:%02X:%02X\n",
    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]
  );

  esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);

  if (esp_now_init() != ESP_OK) {
    Serial.println(F("ERROR: ESP-NOW init failed"));
    drawStatus("RADIO ERROR", 0, false, false);
    while (true) delay(1000);
  }

  esp_now_register_recv_cb(onDataReceived);

  Serial.println(F("Receiver ready. Waiting for ESP8266 packets."));
  drawStatus("WAITING BUS", 0, false, false);
}

void loop() {
  float deviation;
  unsigned long packetTime;
  bool havePacket;

  portENTER_CRITICAL(&packetMux);
  deviation = latestDeviation;
  packetTime = lastPacketMs;
  havePacket = packetReceived;
  portEXIT_CRITICAL(&packetMux);

  const bool connected =
    havePacket && (millis() - packetTime <= LINK_TIMEOUT_MS);

  if (!connected) {
    alarmActive = false;
  } else {
    if (!alarmActive && deviation >= ALARM_ON_COUNTS) {
      alarmActive = true;
    } else if (alarmActive && deviation < ALARM_OFF_COUNTS) {
      alarmActive = false;
    }
  }

  // Continuous ON while warning is active.
  // This assumes a small active buzzer that accepts a HIGH level.
  digitalWrite(BUZZER_PIN, connected && alarmActive ? HIGH : LOW);

  if (!connected) {
    drawStatus("LINK LOST", deviation, false, false);
  } else if (deviation >= CRITICAL_COUNTS) {
    drawStatus("BUS TILTED", deviation, true, alarmActive);
  } else if (deviation >= WARNING_COUNTS) {
    drawStatus("TILT WARNING", deviation, true, alarmActive);
  } else if (deviation >= ALARM_ON_COUNTS) {
    drawStatus("SLIGHT TILT", deviation, true, alarmActive);
  } else {
    drawStatus("BUS STABLE", deviation, true, false);
  }

  delay(80);
}
