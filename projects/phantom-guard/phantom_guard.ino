/*
  PHANTOM GUARD
  ESP32 + RC522 RFID + PIR + SSD1306 OLED + active buzzer

  Demo firmware. UID-only matching is NOT secure authentication.
  Do not use this prototype to protect real property or control a real lock.

  Tested configuration:
    ESP32 Arduino core 3.3.12
    MFRC522, Adafruit GFX, Adafruit SSD1306 libraries

  Pins:
    OLED SDA 21, SCL 22
    RC522 SS 5, SCK 18, MOSI 23, MISO 19, RST 4
    PIR OUT 27
    Active buzzer 26
*/

#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED
#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_ADDR 0x3C

// RC522
#define RFID_SS 5
#define RFID_RST 4

// Other inputs/outputs
#define PIR_PIN 27
#define BUZZER_PIN 26

Adafruit_SSD1306 display(128, 64, &Wire, -1);
MFRC522 rfid(RFID_SS, RFID_RST);

// Authorized demo card UID
const char AUTHORIZED_UID[] = "FB 70 75 63";

bool motionActive = false;
unsigned long lastMotionTime = 0;
unsigned long lastPirScreenUpdate = 0;

void showScreen(const char* title, const char* message) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 15);
  display.println(title);
  display.setCursor(0, 35);
  display.println(message);
  display.display();
}

void successBeep() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(150);
  digitalWrite(BUZZER_PIN, LOW);
}

void deniedBeep() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(150);
    digitalWrite(BUZZER_PIN, LOW);
    if (i < 2) delay(100);
  }
}

String readCardUID() {
  String uid = "";

  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) {
      uid += "0";
    }

    if (i > 0) uid += " ";
    uid += String(rfid.uid.uidByte[i], HEX);
  }

  uid.toUpperCase();
  return uid;
}

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED initialization failed. Check wiring/address.");
    while (true) delay(1000);
  }

  showScreen("PHANTOM GUARD", "Starting...");
  Serial.println();
  Serial.println("PHANTOM GUARD starting");

  SPI.begin(18, 19, 23, RFID_SS);
  rfid.PCD_Init();
  delay(100);

  byte version = rfid.PCD_ReadRegister(MFRC522::VersionReg);
  Serial.print("RC522 version: 0x");
  Serial.println(version, HEX);

  if (version == 0x00 || version == 0xFF) {
    showScreen("RFID ERROR", "Check RC522 wiring");
    Serial.println("RC522 communication error");
  } else {
    showScreen("SYSTEM READY", "Monitoring...");
    Serial.println("RFID reader ready");
  }

  Serial.println("Serial Monitor baud: 115200");
}

void loop() {
  // PIR motion indication
  if (digitalRead(PIR_PIN) == HIGH) {
    lastMotionTime = millis();

    if (!motionActive) {
      motionActive = true;
      showScreen("MOVEMENT DETECTED", "Scan your card");
      Serial.println("Motion detected");
    }
  }

  if (motionActive && millis() - lastMotionTime >= 5000) {
    motionActive = false;
    showScreen("SYSTEM READY", "Monitoring...");
  }

  // Read a newly presented RFID card
  if (!rfid.PICC_IsNewCardPresent()) {
    delay(20);
    return;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    delay(20);
    return;
  }

  String scannedUID = readCardUID();

  Serial.print("Card UID: ");
  Serial.println(scannedUID);

  if (scannedUID.equals(AUTHORIZED_UID)) {
    showScreen("ACCESS GRANTED", "Welcome!");
    Serial.println("Access granted");
    successBeep();
  } else {
    showScreen("ACCESS DENIED", "Unknown card");
    Serial.println("Access denied");
    deniedBeep();
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  delay(1200);
  showScreen("SYSTEM READY", "Monitoring...");
  motionActive = false;
}
