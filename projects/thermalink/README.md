# THERMALINK — Wireless Machine Ambient-Heat Monitor

<sub><b>Project by:</b> Axat Gadhwal · <b>Boards:</b> ESP8266 + ESP32 · <b>Link:</b> ESP-NOW</sub>

THERMALINK monitors the air temperature around a demonstration machine. An ESP8266 reads a DHT11 sensor and sends temperature and humidity readings wirelessly to an ESP32 control station. The ESP32 displays the live readings on an SSD1306 OLED and activates an audible warning using an active buzzer when the configured temperature threshold is reached.

> **Measurement limitation:** The DHT11 measures surrounding air temperature, not a machine's surface, motor winding, bearing, or internal temperature. This is an educational ambient-heat monitoring prototype, not a certified industrial protection or fire-detection system.

## Demonstration features

- Live ambient temperature and relative-humidity readings from DHT11.
- Wireless ESP-NOW telemetry between the machine-side node and control station.
- OLED dashboard showing link state, readings, alert state, and packet sequence.
- Configurable warning and critical temperature thresholds.
- Audible warning through a small active buzzer at the ESP32 station.
- Sensor-error and communication-timeout handling.

## System architecture

```text
  DEMONSTRATION MACHINE AREA
  DHT11
    │
    ▼
  ESP8266 NODEMCU
    │
    │ ESP-NOW · Wi-Fi channel 1
    ▼
  ESP32 CONTROL STATION
    ├── SSD1306 OLED dashboard
    └── Active buzzer alert
```

## Hardware

| Part | Role |
|---|---|
| LOLIN NodeMCU V1.0 / ESP8266 | Reads DHT11 and transmits packets |
| DHT11 | Measures ambient air temperature and humidity |
| ESP32 Dev Module | Receives packets and classifies temperature |
| 0.96-inch SSD1306 I²C OLED | Displays live monitoring data |
| Small active buzzer | Sounds during WARNING and CRITICAL states |
| USB power for each board | Powers the two independent nodes |

No external 5 V supply is assumed. The DHT11 module is powered from the ESP8266 3.3 V pin. The ESP32 and ESP8266 are separate USB-powered nodes; they do not need a shared signal wire or common ground because communication is wireless.

## Wiring

### DHT11 → ESP8266 NodeMCU

| DHT11 module pin | NodeMCU pin |
|---|---|
| VCC / + | 3V3 |
| DATA / OUT | D2 (GPIO4) |
| GND / − | GND |

This table assumes a three-pin DHT11 module with an onboard pull-up resistor. For a bare four-pin DHT11 sensor, follow its pinout and use the required data pull-up resistor. Do not guess the physical pin order; follow the labels on the module.

### OLED → ESP32

| OLED pin | ESP32 pin |
|---|---|
| GND | GND |
| VDD | 3V3 |
| SDA | GPIO 21 |
| SCK / SCL | GPIO 22 |

The receiver sketch uses I²C address `0x3C`. If the display uses `0x3D`, change `OLED_ADDRESS` in the receiver sketch.

### Active buzzer → ESP32

| Buzzer pin | ESP32 pin |
|---|---|
| Positive (+) | GPIO 26 |
| Negative (−) | GND |

This direct connection is only for a small, low-current active buzzer explicitly suitable for a 3.3 V GPIO. If the buzzer's voltage/current requirements are unknown, leave it disconnected until checked. A higher-current buzzer requires a suitable transistor driver and separate power arrangement; never draw a high-current load directly from a GPIO.

## Temperature states

The current firmware uses these demonstration thresholds:

| DHT11 temperature | OLED state | Buzzer |
|---:|---|---|
| Below 31°C | NORMAL | Off |
| 31°C to below 40°C | WARNING | On |
| 40°C or above | CRITICAL | On |
| Invalid DHT11 reading | SENSOR ERROR | Off |
| No packet for 10 seconds | NODE OFFLINE | Off |

The 31°C warning and 40°C critical values are configurable demonstration thresholds. They are not universal industrial temperature limits. Adjust them only for the intended demonstration environment.

## Arduino IDE setup

### ESP8266 transmitter

- Board: LOLIN NodeMCU V1.0 (ESP-12E Module) or the matching NodeMCU option in the installed ESP8266 board package.
- Libraries:
  - DHT sensor library by Adafruit
  - Adafruit Unified Sensor
- Sketch: `transmitter/transmitter.ino`

### ESP32 receiver

- Board: ESP32 Dev Module.
- ESP32 Arduino core: 3.x (the receiver uses the core 3.x ESP-NOW receive-callback signature).
- Libraries:
  - Adafruit GFX Library
  - Adafruit SSD1306
- Sketch: `receiver/receiver.ino`

Install libraries through **Sketch → Include Library → Manage Libraries**. ESP-NOW is included with the respective board packages.

## Configure and start the wireless link

1. Wire the OLED to the ESP32 and upload `receiver/receiver.ino`.
2. Open the ESP32 Serial Monitor at **115200 baud**.
3. Copy the printed `ESP32 STA MAC`. The receiver used for this build printed `70:4B:CA:17:33:98`.
4. Confirm that the same address appears in `RECEIVER_MAC` in the ESP8266 transmitter sketch.
5. Wire the DHT11 to the ESP8266 and upload `transmitter/transmitter.ino`.
6. Open the ESP8266 Serial Monitor at **115200 baud**.
7. The transmitter should report DHT11 readings and queued packet transmissions. The ESP32 OLED should show the readings and ONLINE state.

Both boards are configured for ESP-NOW channel 1. If you replace the ESP32 board, read its new STA MAC and update the transmitter.

## OLED dashboard

Example display:

```text
THERMALINK v1.1
Link: ONLINE
Temp: 32.0 C
Humidity: 45 %
Status: WARNING
Packet: 0012
```

The packet sequence increments with each transmission. The receiver marks the node offline if no packet has arrived for 10 seconds.

## Testing the alert states

For safe and repeatable exhibition testing, use the transmitter's software demonstration mode or temporarily substitute fixed test values in the transmitter sketch. Restore the DHT11 calls before recording real sensor data.

Do not use a flame, hot air gun, hair dryer, or actual overheating machinery to force a high reading. The DHT11 is slow and is intended for ambient monitoring. A hand-warmed sensor may respond gradually, but it is not a calibrated test method.

## Repository files

- `transmitter/transmitter.ino` — ESP8266 DHT11 sender.
- `receiver/receiver.ino` — ESP32 OLED, status logic, and buzzer receiver.
- `wiring.md` — condensed pin map and setup checklist.

## Build photos and video

The following media were supplied with the project update and are embedded here as the THERMALINK build gallery.

### Project video

https://github.com/user-attachments/assets/f53c9482-5399-484a-8882-73c331849f20

### Build images

<img width="100%" alt="THERMALINK build image 1" src="https://github.com/user-attachments/assets/b711ff81-3d77-4943-b6a9-ff9873cb9551" />

<img width="100%" alt="THERMALINK build image 2" src="https://github.com/user-attachments/assets/583a906a-8ed9-44b5-ba2a-4fd37a28afb9" />

<img width="100%" alt="THERMALINK build image 3" src="https://github.com/user-attachments/assets/b6059fac-8bbe-4a72-952b-d52be2761c99" />

<img width="100%" alt="THERMALINK build image 4" src="https://github.com/user-attachments/assets/4d68ff32-6899-4096-99c7-9c2a4abe2150" />

<img width="100%" alt="THERMALINK build image 5" src="https://github.com/user-attachments/assets/773226de-9697-4fff-98e6-cdf2e269baf4" />

<img width="100%" alt="THERMALINK build image 6" src="https://github.com/user-attachments/assets/b1dd10f0-88d4-4e64-92e3-767fcde6c501" />

## Current scope and limitations

THERMALINK demonstrates ambient-temperature telemetry and local warning logic. DHT11 accuracy, response time, and operating range make it unsuitable for fast machine-fault detection. The prototype does not shut down equipment, measure internal machine temperature, or replace industrial sensors, controllers, alarms, or protective systems.

---

[Back to the ESP32 Projects repository](../../README.md)
