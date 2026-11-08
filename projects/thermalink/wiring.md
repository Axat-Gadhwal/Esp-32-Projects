# THERMALINK wiring reference

## Board roles

- ESP8266 NodeMCU: reads the DHT11 and sends telemetry.
- ESP32: receives ESP-NOW packets, updates the OLED, and controls the buzzer.

## DHT11 → ESP8266

| DHT11 module | NodeMCU |
|---|---|
| VCC / + | 3V3 |
| DATA / OUT | D2 (GPIO4) |
| GND / − | GND |

Assumes a three-pin module with an onboard pull-up. Check the labels printed on your module. A bare four-pin DHT11 may require a pull-up resistor.

## OLED → ESP32

| OLED | ESP32 |
|---|---|
| GND | GND |
| VDD | 3V3 |
| SDA | GPIO 21 |
| SCK / SCL | GPIO 22 |

Default I²C address: 0x3C. Change OLED_ADDRESS in the receiver sketch to 0x3D if needed.

## Active buzzer → ESP32

| Buzzer | ESP32 |
|---|---|
| + | GPIO 26 |
| − | GND |

Direct GPIO connection is only for a small, low-current active buzzer rated for 3.3 V GPIO drive. If its requirements are unknown, do not connect it directly. Use a suitable driver for a higher-current buzzer.

## ESP-NOW setup

1. Upload the ESP32 receiver first.
2. Open Serial Monitor at 115200 baud.
3. Copy the printed ESP32 STA MAC.
4. Set that exact address in RECEIVER_MAC in the ESP8266 transmitter.
5. Keep both devices on channel 1.
6. Upload the transmitter.

The ESP32 STA MAC used in this build is 70:4B:CA:17:33:98.

## Status thresholds

- Below 31°C: NORMAL, buzzer OFF.
- 31°C to below 40°C: WARNING, buzzer ON.
- 40°C or above: CRITICAL, buzzer ON.
- Invalid sensor reading: SENSOR ERROR, buzzer OFF.
- No packet for 10 seconds: NODE OFFLINE, buzzer OFF.

These are demonstration thresholds, not certified machine-safety limits.

## Troubleshooting

- **No readings:** check DHT11 VCC, GND, DATA on D2, and install Adafruit DHT plus Adafruit Unified Sensor libraries.
- **ESP-NOW not connecting:** confirm the ESP32 STA MAC, channel 1, and both boards are powered.
- **OLED blank:** check 3V3, GND, SDA 21, SCL 22, and address 0x3C/0x3D.
- **Buzzer silent:** confirm it is an active buzzer and electrically compatible with direct GPIO drive.
- **Node offline:** the receiver expects a packet every 3 seconds and marks the node offline after 10 seconds without a packet.

## Safety and measurement limits

DHT11 measures surrounding air temperature, not machine surface or internal temperature. Do not test with flames, hot air guns, or actual overheating equipment. This is an educational demonstration only.
