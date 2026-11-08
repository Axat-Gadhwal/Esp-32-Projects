# TILTGUARD wiring and troubleshooting

## Board roles

- LOLIN NodeMCU V1.0 (ESP-12E): GY-61 sensor transmitter.
- ESP32 Dev Module: OLED and buzzer receiver.

## GY-61 to NodeMCU

| GY-61 | NodeMCU |
|---|---|
| VCC | 3V3 |
| GND | GND |
| XOUT | A0, only after confirming the exact board's A0 maximum |
| YOUT | Unconnected |
| ZOUT | Unconnected |

**Do not connect XOUT until you have verified the NodeMCU A0 voltage range.** A bare ESP8266 ADC is limited to 1.0 V; board-level dividers differ between models and clones.

## OLED and buzzer to ESP32

| Device | ESP32 |
|---|---|
| OLED VDD | 3V3 |
| OLED GND | GND |
| OLED SDA | GPIO 21 |
| OLED SCL/SCK | GPIO 22 |
| Small active buzzer + | GPIO 26, only if GPIO-compatible |
| Small active buzzer - | GND |

Leave the buzzer disconnected during initial radio testing if its current requirement is unknown. A larger buzzer needs a suitable driver and must not be powered directly from a GPIO.

## Radio setup

1. Flash receiver firmware to ESP32.
2. Open ESP32 Serial Monitor at 115200 baud.
3. Copy the printed `ESP32 STA MAC`.
4. Convert the six pairs to hex bytes and replace `RECEIVER_MAC` in the ESP8266 sketch.
5. Keep channel 1 selected in both sketches.
6. Flash the ESP8266 transmitter and restart both boards.

Example MAC conversion:
`AA:BB:CC:11:22:33` becomes:
`{ 0xAA, 0xBB, 0xCC, 0x11, 0x22, 0x33 }`

## Expected receiver display

- WAITING BUS: no valid packet received yet.
- BUS STABLE: packet received and ADC deviation below trigger.
- SLIGHT TILT: deviation reached 5 ADC counts.
- TILT WARNING: deviation reached 15 ADC counts.
- BUS TILTED: deviation reached 25 ADC counts.
- LINK LOST: no valid packet for 1.5 seconds.

The buzzer is continuously HIGH while the reading is at or above 5 counts; it switches off only after it drops below 3 counts. These are demonstration ADC-count thresholds, not degrees or real-world safety limits.

## Troubleshooting

### LINK LOST
- Check both boards are powered.
- Check ESP32 Serial Monitor prints a nonzero STA MAC.
- Verify that exact MAC is entered in the ESP8266 sketch.
- Check channel 1 is used by both devices.
- Confirm both sketches have identical packet fields and magic value.

### ESP32 keeps rebooting
- Disconnect the buzzer and any other peripherals.
- Test the ESP32 by itself with a minimal sketch.
- Check USB cable and power source.
- Do not repeatedly change multiple variables at once.

### OLED blank
- Check SDA 21, SCL 22, 3V3 and GND.
- If needed, change OLED_ADDRESS from 0x3C to 0x3D.

### False or noisy tilt alerts
- Keep the sensor stationary during its startup zero calibration.
- Secure the sensor so it cannot wobble.
- Increase ALARM_ON_COUNTS / WARNING_COUNTS / CRITICAL_COUNTS if the normal stationary noise reaches the threshold.
- Do not treat this single-axis accelerometer as a reliable moving-vehicle tilt detector.
