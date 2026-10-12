# ESP32-C6 Demo Firmware & Binary Asset Guide

This directory contains precompiled firmware binaries for the **ESP32-C6** slave target, designed to allow immediate out-of-the-box testing with **ESP-ISP** without requiring you to set up an ESP-IDF toolchain.

---

## 1. Demo Firmware Overview

The binaries in [`demo_c6/`](demo_c6/) constitute a complete, production-structured ESP-IDF application built with **ESP-IDF v5.5.5**.

### Memory Layout & Partition Offsets

| Binary File | Flash Offset | Size (Bytes) | SHA-256 Digest |
|---|:---:|---:|---|
| `bootloader.bin` | `0x0000` | 14,320 | `a4cf0dd5b20677aa48995a97576beea7d6c6e73e979be6a89c93a89ee161b9a9` |
| `partition-table.bin` | `0x8000` | 3,072 | `f7344933924fe1431607593c66cf1088bc8a906cfceee15db011ca18d7f273ea` |
| `esp_flasher_demo.bin` | `0x10000` | 138,320 | `ba36eb0092f69046755498877ae8fa9bbd4c62bf6a96860da1dcfeff1a6c0c2a` |

> [!WARNING]
> These binaries are compiled specifically for **ESP32-C6** (RISC-V RV32IMAC). Do **NOT** flash these images to ESP32, ESP32-S3, ESP32-C3, or ESP32-C2 targets, as their CPU instruction sets and register maps differ.

---

## 2. Demo Application Behavior

Once flashed to an ESP32-C6 and reset into normal execution mode:
- **Status LED:** Blinks periodically on **GPIO 7**.
- **UART Communication:** Configured on **UART0** (TX = `GPIO 16`, RX = `GPIO 17`) at `115200` baud (8-N-1).
- **Interactive Version Query:** When the slave receives the string `VERSION\n` over UART0, it responds with:
  ```text
  ESPFLASH-DEMO/1.0.0
  ```
- **Diagnostics:** Verified automatically by [`examples/05_ISP_Hardware_Wiring_Diagnostic`](../examples/05_ISP_Hardware_Wiring_Diagnostic).

---

## 3. Flashing with ESP-ISP (Master MCU)

The bundled examples embed these exact binary images in program memory (`PROGMEM`) via `DemoC6Firmware.h`:
- [`01_ISP_QuickStart_PROGMEM`](../examples/01_ISP_QuickStart_PROGMEM): Demonstrates flashing the complete application image to `0x10000`.
- [`04_ISP_MultiRegion_Flashing`](../examples/04_ISP_MultiRegion_Flashing): Demonstrates writing all three regions (`0x0`, `0x8000`, `0x10000`) in a single orchestrated operation.

### Programmatic Example

```cpp
#include <ESP_ISP.h>
#include "DemoC6Firmware.h"

ESP_ISP isp(Serial1, GPIO_BOOT, GPIO_EN);

void setup() {
    Serial.begin(115200);
    isp.begin(115200);

    // Multi-region flashing: Bootloader, Partition Table, Application
    const esp_isp_region_t regions[] = {
        { 0x00000, demo_image_0, sizeof(demo_image_0), demo_md5_0 },
        { 0x08000, demo_image_1, sizeof(demo_image_1), demo_md5_1 },
        { 0x10000, demo_image_2, sizeof(demo_image_2), demo_md5_2 }
    };

    if (isp.flashRegions(regions, 3, 921600)) {
        Serial.println("ESP32-C6 flashed and verified successfully!");
        isp.resetTarget();
    }
}
```

---

## 4. Standalone USB-UART Recovery (Manual esptool)

If you wish to flash or unbrick an ESP32-C6 directly from your PC using a USB-to-UART adapter (e.g., CP2102, CH340, FTDI) and standard Espressif `esptool`:

```bash
# Navigate to this directory
cd firmware/

# Execute write-flash command
python -m esptool --chip esp32c6 --port COMx --baud 460800 write-flash \
  0x0 demo_c6/bootloader.bin \
  0x8000 demo_c6/partition-table.bin \
  0x10000 demo_c6/esp_flasher_demo.bin
```

*(Replace `COMx` with your actual serial port name, e.g., `/dev/ttyUSB0` on Linux/macOS).*

---

## 5. How to Prepare Your Own Custom Firmware for ESP-ISP

To flash your own custom application using ESP-ISP:

### Step 1: Locate Your Compiled Binaries
After building your project in Arduino IDE or PlatformIO, locate the compiled `.bin` file in the build output directory:
- **PlatformIO:** `.pio/build/<env_name>/firmware.bin`
- **Arduino IDE:** Look at the verbose compilation output for the temporary build directory.

### Step 2: Convert `.bin` to a C Header Array
You can convert any binary file into a C header file using a Python one-liner or `xxd`:

**Using Python:**
```python
import hashlib, sys

data = open("firmware.bin", "rb").read()
md5 = hashlib.md5(data).digest()

with open("MyFirmware.h", "w") as f:
    f.write("#pragma once\n#include <Arduino.h>\n\n")
    f.write(f"const uint32_t my_firmware_size = {len(data)};\n\n")
    f.write("const uint8_t my_firmware_data[] PROGMEM = {\n  ")
    for i, b in enumerate(data):
        f.write(f"0x{b:02x}, ")
        if (i + 1) % 16 == 0:
            f.write("\n  ")
    f.write("\n};\n\n")
    f.write("const uint8_t my_firmware_md5[16] = {\n  " + ", ".join(f"0x{b:02x}" for b in md5) + "\n};\n")

print(f"Generated MyFirmware.h ({len(data)} bytes, MD5: {hashlib.md5(data).hexdigest()})")
```

**Using `xxd` (Linux/macOS):**
```bash
xxd -i firmware.bin > MyFirmware.h
```

### Step 3: Standard Flash Offsets by Chip Family

When flashing individual applications or complete partitions, ensure you use the correct flash offset:

| Target Chip Family | Bootloader Offset | Partition Table Offset | Default App Offset |
|---|:---:|:---:|:---:|
| **ESP32** (Original) | `0x1000` | `0x8000` | `0x10000` |
| **ESP32-S2** | `0x1000` | `0x8000` | `0x10000` |
| **ESP32-S3** | `0x0000` | `0x8000` | `0x10000` |
| **ESP32-C2** | `0x0000` | `0x8000` | `0x10000` |
| **ESP32-C3** | `0x0000` | `0x8000` | `0x10000` |
| **ESP32-C5** | `0x0000` | `0x8000` | `0x10000` |
| **ESP32-C6** | `0x0000` | `0x8000` | `0x10000` |
| **ESP32-P4** | `0x0000` | `0x8000` | `0x10000` |

---

## 6. Hướng Dẫn Firmware & File Nhị Phân (Tiếng Việt)

- Thư mục [`demo_c6/`](demo_c6/) chứa firmware mẫu hoàn chỉnh cho vi điều khiển **ESP32-C6** được build từ ESP-IDF v5.5.5.
- Cấu trúc bộ nhớ flash gồm: `bootloader.bin` (tại `0x0`), `partition-table.bin` (tại `0x8000`), và `esp_flasher_demo.bin` (tại `0x10000`).
- Khi khởi động thành công, LED nhấp nháy tại **GPIO 7**, UART0 hoạt động tại **TX GPIO 16, RX GPIO 17** ở baud 115200. Gửi chuỗi `VERSION\n` sẽ nhận phản hồi `ESPFLASH-DEMO/1.0.0`.
- Để chuyển đổi firmware tự phát triển (`.bin`) thành mảng C trong bộ nhớ flash (`PROGMEM`), sử dụng script Python được cung cấp ở trên hoặc công cụ `xxd`.
- Chú ý bảng offset chuẩn: Các dòng RISC-V (C2, C3, C5, C6) và ESP32-S3 sử dụng bootloader tại offset `0x0000`, trong khi dòng ESP32 cổ điển và ESP32-S2 sử dụng offset `0x1000`.
