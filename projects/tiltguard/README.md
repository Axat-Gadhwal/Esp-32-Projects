# TILTGUARD — Wireless Bus Tilt Demonstrator

<sub><b>Project by:</b> Axat Gadhwal · <b>Type:</b> ESP-NOW two-board embedded prototype</sub>

## Project photos

<img width="100%" alt="TILTGUARD project photo 1" src="https://github.com/user-attachments/assets/180ba8b3-38c3-4469-b375-dde0401b462a" />

<img width="100%" alt="TILTGUARD project photo 2" src="https://github.com/user-attachments/assets/0da1aab5-3f18-46c7-9561-5181ccab1958" />

<img width="100%" alt="TILTGUARD project photo 3" src="https://github.com/user-attachments/assets/c2d6db38-9bb7-4475-b68a-bfaadca9dfa0" />

<img width="100%" alt="TILTGUARD project photo 4" src="https://github.com/user-attachments/assets/f4411053-0374-4dfe-8f52-050e28eccd27" />

TILTGUARD is a two-unit educational prototype that sends tilt-sensor readings from a model bus unit to a remote warning centre.

## Architecture

- **Bus unit:** LOLIN NodeMCU V1.0 (ESP-12E / ESP8266MOD) + GY-61 (ADXL335) accelerometer.
- **Warning centre:** ESP32 + 0.96-inch SSD1306 I²C OLED + optional small active buzzer.
- **Wireless link:** ESP-NOW, Wi-Fi channel 1. No router or internet connection is needed.

The OLED and buzzer belong to the ESP32 warning-centre unit.

## Important safety and accuracy limits

This is a **stationary tabletop/model demonstration only**. Do not install it on a real bus or use it to make transport or safety decisions.

The GY-61 contains an ADXL335 accelerometer, not a gyroscope. A single X-axis reading cannot reliably distinguish static tilt from acceleration, braking, cornering, vibration, or sensor movement. The firmware therefore reports **ADC deviation counts**, not calibrated degrees. The warning thresholds are adjustable demonstration values, not vehicle-safety limits.

Before connecting XOUT to A0, verify the analog-input range of the exact NodeMCU board revision. The bare ESP8266 ADC input is limited to 1.0 V; development boards add voltage-divider circuitry, but clone/revision ranges can vary. Never exceed the board's documented A0 maximum.

Use only a small, low-current active buzzer suitable for direct GPIO drive. If its current requirement is unknown, leave it disconnected during initial testing. Do not power motors, servos, or high-current loads from GPIO pins.

## Repository contents

- `transmitter/transmitter.ino` — ESP8266 sensor sender.
- `receiver/receiver.ino` — ESP32 OLED and buzzer receiver.
- `wiring.md` — pin map, setup, and troubleshooting.

## Wiring

### Bus unit: GY-61 → LOLIN NodeMCU V1.0

| GY-61 | NodeMCU |
|---|---|
| VCC | 3V3 |
| GND | GND |
| XOUT | A0 — only after checking the board's A0 voltage limit |
| YOUT | Not connected |
| ZOUT | Not connected |

Align the sensor's X axis across the width of the tabletop model. Keep it stationary and level while the transmitter starts and performs its zero-offset calibration.

### Warning centre: OLED and buzzer → ESP32

| Component | ESP32 |
|---|---|
| OLED VDD | 3V3 |
| OLED GND | GND |
| OLED SDA | GPIO 21 |
| OLED SCL / SCK | GPIO 22 |
| Active buzzer + | GPIO 26, only if low-current and GPIO-compatible |
| Active buzzer − | GND |

The ESP8266 and ESP32 are separate USB-powered units. ESP-NOW is wireless; no signal wire or shared ground between the two boards is needed.

## Arduino IDE setup

### ESP8266 transmitter
- Board: LOLIN(WEMOS) D1 R2 & mini or the matching LOLIN NodeMCU/ESP-12E board option available in your ESP8266 package.
- Install the ESP8266 board package.
- Upload `transmitter/transmitter.ino`.

### ESP32 receiver
- Board: ESP32 Dev Module.
- ESP32 Arduino core: 3.x (receiver callback uses the core 3.x API).
- Libraries:
  - Adafruit GFX Library
  - Adafruit SSD1306
- Upload `receiver/receiver.ino`.

## Configure the wireless link

1. Upload the receiver and open its Serial Monitor at 115200 baud.
2. Copy the **STA MAC** printed by the ESP32.
3. Paste its six hexadecimal bytes into `RECEIVER_MAC` in the ESP8266 transmitter.
4. Keep both devices on channel 1.
5. Restart the receiver, then the transmitter.
6. The receiver should change from **WAITING FOR BUS** to **BUS STABLE** after valid packets arrive.

The receiver prints its station MAC using the ESP32 MAC API after Wi-Fi station mode is enabled. Do not use an all-zero MAC address.

## Warning logic

The transmitter sends a filtered ADC deviation value every 100 ms. The receiver uses these default demonstration thresholds:

| ADC deviation | OLED state | Buzzer |
|---:|---|---|
| Below 5 counts | BUS STABLE | Off |
| 5–14 counts | SLIGHT TILT | Continuous |
| 15–24 counts | TILT WARNING | Continuous |
| 25 counts or more | BUS TILTED | Continuous |

The alarm turns on at 5 counts and turns off only after the reading falls below 3 counts. This hysteresis reduces rapid switching near the threshold. These are raw ADC-count thresholds, **not degrees**. Tune them only for a controlled tabletop demonstration after observing the sensor's normal noise.

If no valid packet arrives for 1.5 seconds, the receiver displays **LINK LOST** and turns the buzzer off.

## Troubleshooting

- **LINK LOST:** Confirm both boards are powered, both use channel 1, and the ESP8266 transmitter contains the exact ESP32 STA MAC printed in Serial Monitor.
- **ESP32 MAC prints zeros:** Confirm Wi-Fi station mode was started before calling the MAC-reading function; use the receiver sketch as supplied.
- **Packets not arriving:** Check that the ESP8266 peer MAC is correct and that the ESP8266 and ESP32 firmware use the same packet structure and channel.
- **OLED blank:** Check 3V3/GND, SDA 21, SCL 22, and I²C address 0x3C. Change `OLED_ADDRESS` to 0x3D if required by the display.
- **Buzzer stays silent:** Confirm it is an active, low-current buzzer compatible with a 3.3 V GPIO. Keep it disconnected if its electrical requirements are unknown.
- **Unstable readings:** Keep the sensor stationary, check its mounting, and adjust `FILTER_ALPHA` or the ADC-count thresholds.

## Current status

Firmware and documentation are an initial reproducible build. The analog scale and warning thresholds still require bench calibration on the exact GY-61 module and NodeMCU revision. Do not interpret the display as an angle measurement until a proper multi-position calibration has been completed.

---

[Back to the ESP32 Projects repository](../../README.md)
