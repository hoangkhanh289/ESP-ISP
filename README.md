# ESP-ISP: Universal In-System Programmer for ESP Microcontrollers

[![Author](https://img.shields.io/badge/Author-Hoang%20Khanh-informational.svg)](https://github.com/hoangkhanh289)
[![GitHub Repo](https://img.shields.io/badge/GitHub-ESP--ISP-blue.svg)](https://github.com/hoangkhanh289/ESP-ISP)
[![Release](https://img.shields.io/badge/Release-v1.0.0-success.svg)](https://github.com/hoangkhanh289/ESP-ISP/releases)
[![License](https://img.shields.io/badge/License-Apache%202.0-green.svg)](LICENSE)
[![Arduino](https://img.shields.io/badge/Arduino-1.5%2B%20Precompiled-blue.svg)](https://github.com/arduino/Arduino/wiki/Arduino-IDE-1.5:-Library-specification)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-Compatible-orange.svg)](extras/platformio.py)
[![Masters](https://img.shields.io/badge/Masters-ESP32%20%7C%20STM32%20%7C%20RP2040-blueviolet.svg)](#supported-master-mcus)
[![Slaves](https://img.shields.io/badge/Slaves-ESP32%20%7C%20C6%20%7C%20C5%20%7C%20C3%20%7C%20S3%20%7C%20P4%20%7C%208266-success.svg)](#supported-slave-coprocessors)

> **Language / Ngôn ngữ:** **English** | [🇻🇳 Tiếng Việt](README.vi.md)

---

## Overview

**ESP-ISP (In-System Programmer)** is a production-grade on-board microcontroller programming library engineered for the Arduino and PlatformIO ecosystems. It enables a host microcontroller (**Master MCU**, such as **ESP32-S3, ESP32-P4, STM32, or RP2040**) to act as an autonomous hardware programmer, flashing, upgrading, and recovering firmware on installed **Slave Coprocessors** (**ESP32-C6, C5, C3, S3, P4, ESP8266**) directly over a serial/UART link:

* ⚡ **True In-System Programming (ISP)**: Eliminates physical BOOT/RESET buttons and dedicated external USB-to-UART bridge ICs (e.g. CP2102, CH340). The master autonomously pulses `RESET / EN` and controls the `BOOT / IO0` strap pin to command the target MCU into ROM Download Mode.
* 🚀 **High-Speed & Secure Flashing**: Begins ROM handshake at a reliable 115,200 bps, injects an ultra-fast RAM stub, negotiates transfer speeds up to **921,600 bps** (or higher), and verifies data integrity with on-chip hardware MD5 checksums.
* 🔄 **Versatile Firmware Sources**: Flash binaries stored in MCU flash arrays (`PROGMEM`), streamed from filesystems (`LittleFS`), read from external storage (`MicroSD Cards`), or delivered over network connections (OTA).
* 📦 **Precompiled Core Engine (Arduino 1.5+ Spec)**: Complex SLIP packet framing, ROM protocol handshakes, and flashing state-machines reside in precompiled, highly optimized static archives (`libesp_isp.a`). This guarantees fast host compilation, zero code bloat, and consistent behavior across architectures.

---

## System Architecture

```text
+------------------------------------+                                   +------------------------------------+
|         MASTER MCU (HOST)          |                                   |       SLAVE ESP (COPROCESSOR)      |
|    ESP32-S3 / ESP32-P4 / STM32     |                                   |    ESP32-C6 / C5 / C3 / S3 / P4    |
|                                    |         Hardware ISP Bus          |                                    |
|  - Arduino Application Sketch      | <===============================> |  - Target System / Bare Metal      |
|  - #include <ESP_ISP.h>            |                                   |  - ROM Bootloader / Download Mode  |
|  - EspISP.begin(config)            |     UART TX --------> ROM RX      |  - RAM Flasher Stub Execution      |
|  - EspISP.connect(target, baud)    |     UART RX <-------- ROM TX      |  - SPI Flash Writing & Partition   |
|  - EspISP.flashStream(...)         |     reset_pin ------> EN / CHIP_PU|                                    |
|  - EspISP.verifyMD5(...)           |     boot_pin -------> BOOT Strap  |                                    |
|  - EspISP.resetTarget()            |     GND ------------> GND         |                                    |
+------------------------------------+                                   +------------------------------------+
```

---

## Hardware Compatibility Matrix

### Supported Master MCUs

| Master MCU Family | Compatible Core | UART Interface | Static Archive Location |
| :--- | :--- | :---: | :--- |
| **ESP32-P4** | Arduino-ESP32 3.x | Native Hardware UART | `src/esp32p4/libesp_isp.a` |
| **ESP32-S3** | Arduino-ESP32 3.x / 2.0.17 | Native Hardware UART | `src/esp32s3/libesp_isp.a` |
| **ESP32 Classic** | Arduino-ESP32 3.x | Native Hardware UART | `src/esp32/libesp_isp.a` |
| **ESP32-C3 / C6 / C5** | Arduino-ESP32 3.x | Native Hardware UART | `src/esp32c3/`, `c6/`, `c5/` |
| **STM32F4 / G4** | STM32duino 3.0+ | `Uart` / `HardwareSerial` | `src/cortex-m4/libesp_isp.a` |
| **STM32F7 / H7** | STM32duino 3.0+ | `Uart` / `HardwareSerial` | `src/cortex-m7/libesp_isp.a` |
| **RP2040 (Raspberry Pi Pico)** | Arduino-Pico (Earle Philhower) 6.x | `SerialUART` | `src/cortex-m0plus/libesp_isp.a` |

### Supported Slave Coprocessors

| Target Slave Chip | Primary BOOT Pin | Auxiliary Strap Requirements | Common Bootloader Offset |
| :--- | :---: | :--- | :---: |
| **ESP32-C6 / C3 / C2 / H2** | `GPIO 9` | `GPIO 8` must be HIGH (internal pull-up or external) | `0x0000` |
| **ESP32-C5** | **`GPIO 28`** | **`GPIO 27` must be HIGH** | **`0x2000`** |
| **ESP32-P4** | **`GPIO 35`** | **`GPIO 36` must be HIGH** | **`0x2000`** |
| **ESP32-S3 / S2** | `GPIO 0` | `GPIO 46` LOW or floating | `0x0000` |
| **ESP32 Classic** | `GPIO 0` | `GPIO 2` LOW or floating, `GPIO 12` LOW | `0x1000` |
| **ESP8266** | `GPIO 0` | `GPIO 2` HIGH, `GPIO 15` LOW | `0x0000` |

> [!IMPORTANT]
> **Key Hardware Considerations**:
> * **ESP32-C5** uses **GPIO 28** for its BOOT strap pin (unlike GPIO 9 on other C-series chips).
> * **ESP32-P4** uses **GPIO 35** for its BOOT strap pin.
> * Always ensure a solid **Common GND** connection between Master and Slave.

---

## Hardware Wiring

### Recommended Schematic

```text
               SLAVE 3.3V                   SLAVE 3.3V
                    |                            |
                  10 kΩ                        10 kΩ
                    |                            |
  Master RESET ---- EN (CHIP_PU)    BOOT Strap --+------- Master BOOT
                    |
              Filter Cap (0.1 µF ~ 1 µF)
                    |
                   GND
```

### Wiring Reference Table

| Signal | Master Pin (Example ESP32-S3) | Slave Pin (Example ESP32-C6) | Technical Description |
| :--- | :---: | :---: | :--- |
| **TX → RX** | `GPIO 17` | `UART0 RX (GPIO 17)` | Master transmits firmware & flash commands |
| **RX ← TX** | `GPIO 18` | `UART0 TX (GPIO 16)` | Master receives ROM handshakes & verification |
| **Reset** | `GPIO 5` | `CHIP_EN` | Active LOW pulse resets the target chip |
| **Boot Strap** | `GPIO 4` | `BOOT (GPIO 9)` | Held LOW during EN release to trigger Bootloader |
| **GND** | `GND` | `GND` | **Mandatory shared ground plane** |

---

## Public API Reference (`ESP_ISP.h`)

Include header: `#include <ESP_ISP.h>`  
Global instance: `extern EspISPClass EspISP;`

### Configuration & Connection

#### `bool begin(const EspISPConfig &config)`
Initializes the ISP engine with GPIO pin assignments, UART peripheral settings, and timeout configuration.
```cpp
EspISPConfig cfg;
cfg.rx_pin = 18;
cfg.tx_pin = 17;
cfg.reset_pin = 5;
cfg.boot_pin = 4;
cfg.flash_baud = 921600;
EspISP.begin(cfg);
```

#### `bool connect(isp_target_chip_t target = ISP_TARGET_AUTO, uint32_t higher_baudrate = 0)`
Automates the reset/boot sequence, syncs with ROM at 115,200 bps, detects target chip ID, injects the RAM flasher stub, and accelerates the link to `flash_baud`.

#### `void end()`
Releases the UART peripheral and GPIO control so the application sketch can reuse the bus for normal runtime communications.

---

### Flashing & Verification

#### `bool flashPROGMEM(uint32_t address, const uint8_t *buffer, size_t size)`
Flashes a binary buffer embedded in master flash memory (e.g. `const uint8_t data[] PROGMEM`).

#### `bool flashStream(uint32_t address, Stream &stream, size_t size)`
Flashes firmware dynamically from any Arduino `Stream` object (e.g. `File` from LittleFS or SD Card, or an incoming network stream).

#### `bool verifyMD5(uint32_t address, size_t size, const uint8_t expected_md5[16])`
#### `bool verifyMD5Hex(uint32_t address, size_t size, const char expected_hex[33])`
Queries the target chip to compute and verify the MD5 hash of the flashed memory region against the reference checksum.

#### `void resetTarget()`
Pulses the target's `EN` pin with `BOOT` high, releasing it into user application mode.

#### `void onProgress(ISPProgressCallback callback)`
Registers a progress callback function `void callback(size_t written, size_t total)` to monitor write percentage.

---

## Quickstart Examples

### Flashing from PROGMEM (Flash Memory)

```cpp
#include <ESP_ISP.h>
#include "DemoC6Firmware.h" // Contains bootloader, partition table, and app arrays

void setup() {
  Serial.begin(115200);
  delay(1500);

  EspISPConfig cfg;
  cfg.rx_pin = 18;
  cfg.tx_pin = 17;
  cfg.reset_pin = 5;
  cfg.boot_pin = 4;
  cfg.flash_baud = 921600;

  EspISP.onProgress([](size_t written, size_t total) {
    Serial.printf("Flashing: %u%%\r", (unsigned)(written * 100 / total));
  });

  if (!EspISP.begin(cfg)) {
    Serial.printf("ISP Init Failed: %s\n", EspISP.getLastError());
    return;
  }

  Serial.println("Connecting to target...");
  if (!EspISP.connect(ISP_TARGET_ESP32C6)) {
    Serial.printf("Connection Failed: %s\n", EspISP.getLastError());
    return;
  }

  Serial.printf("Target: %s | Flash Size: %u MB\n", 
                EspISP.getTargetName(), 
                EspISP.getFlashSize() / (1024 * 1024));

  // Flash each partition image
  for (const auto &img : demo_images) {
    Serial.printf("Flashing partition @ 0x%05X (%u bytes)...\n", img.offset, img.size);
    if (!EspISP.flashPROGMEM(img.offset, img.data, img.size) ||
        !EspISP.verifyMD5(img.offset, img.size, img.md5)) {
      Serial.printf("Flashing Error: %s\n", EspISP.getLastError());
      return;
    }
  }

  Serial.println("\nFlashing Complete! Booting application...");
  EspISP.resetTarget();
  EspISP.end();
}

void loop() {
  delay(1000);
}
```

### Flashing from LittleFS / MicroSD

```cpp
#include <ESP_ISP.h>
#include <LittleFS.h>

void flashFromFilesystem() {
  File file = LittleFS.open("/firmware.bin", "r");
  if (!file || file.size() == 0) {
    Serial.println("Firmware file not found!");
    return;
  }

  // Flash application partition at offset 0x10000
  if (EspISP.flashStream(0x10000, file, file.size())) {
    Serial.println("App updated successfully!");
    EspISP.resetTarget();
  } else {
    Serial.printf("Flashing Failed: %s\n", EspISP.getLastError());
  }
  file.close();
}
```

---

## Bundled Examples Index

Detailed examples can be found in the [`examples/`](examples/) directory:

| Example Folder | Primary Feature | Real-World Application |
| :--- | :--- | :--- |
| [`01_ISP_QuickStart_PROGMEM`](examples/01_ISP_QuickStart_PROGMEM/) | **PROGMEM Flashing** | Self-contained provisioning: Flashes Bootloader, Partition Table, and App from embedded C arrays. |
| [`02_ISP_Flash_From_LittleFS`](examples/02_ISP_Flash_From_LittleFS/) | **Filesystem Flashing** | Updates slave firmware directly from a `.bin` file stored in the master's internal flash. |
| [`03_ISP_Flash_From_SDCard`](examples/03_ISP_Flash_From_SDCard/) | **SD Card Flashing** | Portable programmer: Read firmware images from a removable microSD card in the field. |
| [`04_ISP_Coprocessor_Auto_Update`](examples/04_ISP_Coprocessor_Auto_Update/) | **Automatic Firmware Sync** | Queries the coprocessor app version at boot and flashes an update only if the version is outdated. |
| [`05_ISP_Hardware_Wiring_Diagnostic`](examples/05_ISP_Hardware_Wiring_Diagnostic/) | **Wiring Diagnostic** | Verifies pin logic levels, detects target chip model, and reads MAC / Flash ID without writing data. |

---

## Installation & Setup

### Arduino IDE
1. Download the release `.zip` package from [Releases](https://github.com/hoangkhanh289/ESP-ISP/releases).
2. In the Arduino IDE, navigate to **Sketch → Include Library → Add .ZIP Library...** and select the downloaded file.
3. Open an example via **File → Examples → ESP-ISP → 01_ISP_QuickStart_PROGMEM**.

### PlatformIO
Add the repository directly to your `platformio.ini`:

```ini
[env:esp32s3]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino
lib_deps =
    https://github.com/hoangkhanh289/ESP-ISP.git
```

For legacy Arduino-ESP32 2.0.17 environments, a dedicated precompiled archive is bundled automatically via `extras/platformio.py`.

---

## Troubleshooting & Diagnostic Codes

| Error Code | Potential Root Cause | Recommended Action |
| :--- | :--- | :--- |
| `ISP_ERROR_TIMEOUT` (2) | Common GND missing, crossed TX/RX lines, insufficient 3.3V current, or reset hold duration too short. | Ensure common ground; verify Master TX → Slave RX; increase `reset_hold_ms` to 150–200 ms; add a 1 µF capacitor on `EN`. |
| `ISP_ERROR_INVALID_MD5` (4) | Data corruption caused by baud rate exceeding bus capability or trace noise. | Reduce `flash_baud` to 921600 or 115200 bps; shorten connection wires; add a 100 nF decoupling capacitor near the slave. |
| `ISP_ERROR_UNSUPPORTED_CHIP` (7) | Target chip not in Download Mode or auxiliary strap pin configured incorrectly. | Verify target BOOT pin (GPIO 9 for C6/C3, GPIO 28 for C5, GPIO 35 for P4); ensure auxiliary strap pins match datasheet. |
| `ISP_ERROR_IMAGE_SIZE` (3) | Image exceeds target physical flash capacity. | Inspect flash size with `getFlashSize()`; verify `partition-table.bin` configuration. |

---

## Verification & Quality Assurance

Every release of **ESP-ISP** is systematically verified across multiple architectures:
* **Arduino CLI 1.5.1**: 15/15 compile & link test cases PASS (ESP32-S3, STM32F446RE, RP2040).
* **PlatformIO Multi-Environment**: 3/3 target builds PASS (Core 2.0.17 legacy S3, STM32duino 3.0.0, Arduino-Pico 6.0.0).
* **Deterministic Release Packaging**: Automated SHA-256 verification and checksum manifests.
* Refer to [VALIDATION.md](docs/VALIDATION.md) for full benchmark logs and test methodology.

---

## Author & License

* 👤 **Author & Maintainer**: **Hoàng Khánh** ([@hoangkhanh289](https://github.com/hoangkhanh289))
* 🌐 **Official Repository**: [https://github.com/hoangkhanh289/ESP-ISP](https://github.com/hoangkhanh289/ESP-ISP)
* 📄 **License**: Released under the **Apache License 2.0**. See [LICENSE](LICENSE) and [NOTICE](NOTICE) for details.
* 🤝 **Attributions**: Built upon open-source foundations from Espressif Systems (`esp-serial-flasher`, `esp-flasher-stub`) and Colin Plumb / Jouni Malinen (MD5). See [Third-Party Notices](docs/THIRD_PARTY_NOTICES.md).
