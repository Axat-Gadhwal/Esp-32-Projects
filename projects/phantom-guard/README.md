# PHANTOM GUARD

**ESP32 RFID access-monitoring prototype**

PHANTOM GUARD is a working educational prototype that combines an RFID reader, motion sensor, OLED display, and buzzer.

## What it does

1. The PIR sensor detects movement and the OLED displays **MOVEMENT DETECTED — Scan your card**.
2. The RC522 reads a card UID.
3. The ESP32 compares the UID with the configured allowed UID.
4. The OLED displays **ACCESS GRANTED** or **ACCESS DENIED**.
5. The buzzer gives one short beep for the allowed UID and three short beeps for other UIDs.

> **Prototype security note:** This project uses the card UID as its identifier. UID-only access checks are not secure authentication: some RFID cards can be copied or emulated. Use this as a learning/demo project, not as a real door lock or security system.

## Components

- ESP32 development board
- MFRC522 / RC522 RFID reader
- 0.96-inch I²C OLED (SSD1306, 128×64)
- PIR motion sensor (e.g. HC-SR501)
- Small, low-current active buzzer suitable for an ESP32 GPIO
- RFID cards/tags, breadboard, jumper wires
- USB cable for the ESP32

The servo is not included in this version.

## Wiring

### OLED → ESP32

| OLED | ESP32 |
|---|---|
| GND | GND |
| VDD | 3V3 |
| SCK / SCL | GPIO 22 |
| SDA | GPIO 21 |

The firmware defaults to OLED address `0x3C`. If your display scanner reports `0x3D`, change `OLED_ADDR` in the sketch.

### RC522 → ESP32

| RC522 | ESP32 |
|---|---|
| SDA / SS | GPIO 5 |
| SCK | GPIO 18 |
| MOSI | GPIO 23 |
| MISO | GPIO 19 |
| RST | GPIO 4 |
| 3.3V | 3V3 |
| GND | GND |
| IRQ | Not connected |

**Important:** The RC522 pin labelled SDA is used as SPI chip-select (SS) in this wiring. It is not connected to the OLED's I²C SDA line. Power the RC522 from 3.3V, never 5V.

### PIR → ESP32

| PIR | ESP32 |
|---|---|
| VCC | 5V pin, if confirmed on your board |
| OUT | GPIO 27 |
| GND | GND |

Check the sensor's output voltage before connecting OUT: the ESP32 GPIO is **not 5V tolerant**. The signal at GPIO 27 must stay at or below 3.3V. Power the PIR only from a suitable 5V pin/source with adequate current capacity. Do not use TX/RX pins as power pins.

### Buzzer → ESP32

For a small, low-current **active** buzzer module/component that is suitable for direct GPIO drive:

| Buzzer | ESP32 |
|---|---|
| + | GPIO 26 |
| − | GND |

Do not drive a high-current buzzer directly from an ESP32 GPIO. Use a suitable driver circuit for a larger buzzer. A passive buzzer may require a tone/PWM signal rather than the simple HIGH/LOW pattern in this firmware.

### Common ground

The OLED, RC522, PIR, and ESP32 must share a common ground. Keep the 3.3V and 5V power rails separate.

## Software setup

Tested project configuration:

- Arduino IDE
- ESP32 board package: 3.3.12
- Board selection: **ESP32 Dev Module**
- Libraries:
  - MFRC522
  - Adafruit GFX Library
  - Adafruit SSD1306

Install libraries using **Sketch → Include Library → Manage Libraries**.

## Upload

1. Open `phantom_guard.ino` in Arduino IDE.
2. Select **ESP32 Dev Module** and the correct COM port.
3. Confirm the pin wiring and power precautions above.
4. Upload the sketch.
5. Open Serial Monitor at **115200 baud**.
6. Scan the authorized keychain card. The current configured UID is `FB 70 75 63`.
7. Scan another card to see the denied response.

To authorize a different card, edit `AUTHORIZED_UID` in the sketch to match the UID printed in Serial Monitor. Keep the spaces and hexadecimal formatting consistent.

## Pin summary

| Function | ESP32 pin |
|---|---:|
| OLED SDA | 21 |
| OLED SCL | 22 |
| RC522 SS | 5 |
| RC522 SCK | 18 |
| RC522 MOSI | 23 |
| RC522 MISO | 19 |
| RC522 RST | 4 |
| PIR OUT | 27 |
| Buzzer | 26 |

## Troubleshooting

- **OLED blank:** Check 3.3V/GND, SDA 21, SCL 22, and the I²C address (`0x3C` or `0x3D`).
- **RC522 not responding:** Check 3.3V power and all SPI wires. Do not power it with 5V.
- **Unreadable Serial Monitor:** Set baud rate to 115200.
- **PIR keeps triggering:** Allow its startup settling time and adjust its sensitivity/delay controls if present.
- **Buzzer silent:** Confirm it is an active buzzer and that its current requirement is suitable for GPIO drive.

## Current scope

This release contains RFID UID checking, PIR motion indication, OLED messages, and buzzer feedback. Servo control is intentionally not included; a servo needs a suitable power arrangement and must not be powered from the ESP32 3.3V pin.
