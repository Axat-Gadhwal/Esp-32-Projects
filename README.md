# ESP32 Projects

A hands-on collection of ESP32 and ESP8266 experiments, embedded systems, and IoT builds by **Axat Gadhwal**.

This repository documents projects that are built and tested with real hardware. Each project folder should contain its firmware, wiring reference, required libraries, and build notes.

## Featured project

### PHANTOM GUARD — RFID Smart Access Monitor

<sub><b>Status:</b> Working prototype · <b>Board:</b> ESP32</sub>

A compact access-monitoring demonstration that combines:

- **RC522 RFID reader** — reads card identifiers and checks an allowed UID.
- **PIR motion sensor** — detects nearby movement and prompts a card scan.
- **0.96-inch I²C OLED** — displays system, motion, and access messages.
- **Buzzer** — gives one short beep for an accepted card and three for a rejected card.

The current prototype uses UID matching for demonstration purposes. It is **not suitable for securing real doors or valuables**; UID-only checks can be copied or emulated on some cards.

**Project files:** [Wiring & build guide](projects/phantom-guard/README.md) · [ESP32 firmware](projects/phantom-guard/phantom_guard.ino)

## Repository structure

```text
Esp-32-Projects/
├── README.md
└── projects/
    └── phantom-guard/
        ├── README.md
        └── phantom_guard.ino
```

## Hardware gallery

Photos from the project work:

<img width="100%" alt="ESP32 project photo 1" src="https://github.com/user-attachments/assets/2af6dde3-83d7-4740-8b50-54d4655e5939" />

<img width="100%" alt="ESP32 project photo 2" src="https://github.com/user-attachments/assets/b6559bcd-3ffe-46df-aa78-46702a0e1233" />

<img width="100%" alt="ESP32 project photo 3" src="https://github.com/user-attachments/assets/67a1734d-1b72-4888-8c79-d3acc8bd0de0" />

<img width="100%" alt="ESP32 project photo 4" src="https://github.com/user-attachments/assets/2fa806d3-5553-403b-b6c1-2618c28bc548" />

<img width="100%" alt="ESP32 project photo 5" src="https://github.com/user-attachments/assets/e65e699e-e5ce-4976-aaaf-9abcf9b49971" />

<img width="100%" alt="ESP32 project photo 6" src="https://github.com/user-attachments/assets/026e3033-5fe7-4ede-987b-b52031086341" />

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
