# ESP32 Projects

A hands-on collection of ESP32 and ESP8266 experiments, embedded systems, and IoT builds by **Axat Gadhwal**.

This repository documents projects built with real hardware. Each project folder contains its firmware, wiring reference, required libraries, and build notes.

## Featured projects

### TILTGUARD — Wireless Bus Tilt Demonstrator

<sub><b>Status:</b> Educational prototype · <b>Boards:</b> ESP8266 + ESP32</sub>

TILTGUARD is a two-unit tabletop model. A GY-61 (ADXL335) accelerometer on a LOLIN NodeMCU sends readings wirelessly over ESP-NOW to an ESP32 warning centre. The ESP32 displays the status on an OLED and can operate a small compatible buzzer.

### TILTGUARD



https://github.com/user-attachments/assets/e9c9109c-6a51-4a09-af01-26bc315057a2



### Latest TILTGUARD build photos

<img width="100%" alt="TILTGUARD project photo 1" src="https://github.com/user-attachments/assets/180ba8b3-38c3-4469-b375-dde0401b462a" />

<img width="100%" alt="TILTGUARD project photo 2" src="https://github.com/user-attachments/assets/0da1aab5-3f18-46c7-9561-5181ccab1958" />

<img width="100%" alt="TILTGUARD project photo 3" src="https://github.com/user-attachments/assets/c2d6db38-9bb7-4475-b68a-bfaadca9dfa0" />

<img width="100%" alt="TILTGUARD project photo 4" src="https://github.com/user-attachments/assets/f4411053-0374-4dfe-8f52-050e28eccd27" />

**Project files:** [TILTGUARD build guide](projects/tiltguard/README.md) · [Wiring guide](projects/tiltguard/wiring.md) · [ESP8266 transmitter](projects/tiltguard/transmitter/transmitter.ino) · [ESP32 receiver](projects/tiltguard/receiver/receiver.ino)

**Important:** This is a stationary educational demonstration, not a vehicle safety system. Its current readings are ADC deviations, not calibrated tilt angles.

### PHANTOM GUARD — RFID Smart Access Monitor

<sub><b>Status:</b> Working prototype · <b>Board:</b> ESP32</sub>

A compact access-monitoring demonstration combining:

- **RC522 RFID reader** — reads card identifiers and checks an allowed UID.
- **PIR motion sensor** — detects nearby movement and prompts a card scan.
- **0.96-inch I²C OLED** — displays system, motion, and access messages.
- **Buzzer** — gives one short beep for an accepted card and three for a rejected card.

The current prototype uses UID matching for demonstration purposes. It is **not suitable for securing real doors or valuables**; UID-only checks can be copied or emulated on some cards.

**Project files:** [PHANTOM GUARD wiring & build guide](projects/phantom-guard/README.md) · [ESP32 firmware](projects/phantom-guard/phantom_guard.ino)

## Repository structure

```text
Esp-32-Projects/
├── README.md
└── projects/
    ├── phantom-guard/
    │   ├── README.md
    │   └── phantom_guard.ino
    └── tiltguard/
        ├── README.md
        ├── wiring.md
        ├── transmitter/
        │   └── transmitter.ino
        └── receiver/
            └── receiver.ino
```

## Hardware gallery

Project photos from the actual builds:

<img width="100%" alt="ESP32 project photo 1" src="https://github.com/user-attachments/assets/2af6dde3-83d7-4740-8b50-54d4655e5939" />

<img width="100%" alt="ESP32 project photo 2" src="https://github.com/user-attachments/assets/b6559bcd-3ffe-46df-aa78-46702a0e1233" />

<img width="100%" alt="ESP32 project photo 3" src="https://github.com/user-attachments/assets/67a1734d-1b72-4888-8c79-d3acc8bd0de0" />

<img width="100%" alt="ESP32 project photo 4" src="https://github.com/user-attachments/assets/2fa806d3-5553-403b-b6c1-2618c28bc548" />

<img width="100%" alt="ESP32 project photo 5" src="https://github.com/user-attachments/assets/e65e699e-e5ce-4976-aaaf-9abcf9b49971" />

<img width="100%" alt="ESP32 project photo 6" src="https://github.com/user-attachments/assets/026e3033-5fe7-4ede-987b-b52031086341" />

## Video

[Watch the project video](https://github.com/user-attachments/assets/754d2961-a0f6-4e0b-9fbf-358a9262af78)

## Build philosophy

- Prefer clear wiring maps and reproducible build steps.
- Record the exact board, libraries, and pin assignments used.
- Keep experimental code separate from tested firmware.
- Document power requirements before connecting peripherals.

## Author

**Axat Gadhwal**  
GitHub: [@Axat-Gadhwal](https://github.com/Axat-Gadhwal)

---

If you try a project, feel free to open an issue with your board details, wiring, and any error messages.
