# Validation & Quality Assurance Report

**Project:** ESP-ISP (Universal High-Speed In-System Programmer Library)  
**Date of Report:** 2026-10-12  
**Validation Artifact:** [`validation-report.json`](validation-report.json)  
**Overall Status:** **PASSED (100% Automated Checks & Compilation Gates)**

---

## 1. Executive Summary

This document records the comprehensive validation results for the **ESP-ISP** library release payload. Testing covers:
1. **Multi-Architecture Compilation & Linkage:** Verified across three major microcontroller families (Xtensa LX7, ARM Cortex-M4F, ARM Cortex-M0+) with 5 bundled examples (15 total cases).
2. **PlatformIO Integration:** Multi-environment matrix compilation validating automated toolchain archive selection and link-time flags.
3. **Protocol & State Machine Simulation:** ROM and stub SLIP handshakes, baud rate switching, checksum verification, and error handling.
4. **Supply Chain & Distribution Integrity:** Strict file allowlisting, archive hash validation against `binary-manifest.json`, and isolated sandbox installation tests.

> [!NOTE]
> The automated suite tests real compiler toolchains, binary linkers, and ROM protocol simulators. Hardware flashing, electrical timing, and physical flash endurance on production PCBs should be verified using [`examples/05_ISP_Hardware_Wiring_Diagnostic`](../examples/05_ISP_Hardware_Wiring_Diagnostic).

---

## 2. Arduino CLI Compilation Matrix

Tested with **Arduino CLI v1.5.1** against official, installed vendor board packages:
- **Arduino-ESP32:** v3.3.12 (and legacy v2.0.17 for ESP32-S3)
- **STM32duino:** v3.0.0 (STM32 Core)
- **Arduino-Pico:** v6.0.0 (Raspberry Pi Pico / RP2040)

### Results Table

| Target Architecture | Target Board / FQBN | Core Version | Examples Tested | Link Status |
|---|---|:---:|:---:|:---:|
| **Espressif Xtensa LX7** | `esp32:esp32:esp32s3` | `3.3.12` | 01, 02, 03, 04, 05 | **5 / 5 PASS** |
| **STMicroelectronics Cortex-M4F** | `STMicroelectronics:stm32:Nucleo_64:pnum=NUCLEO_F446RE` | `3.0.0` | 01, 02, 03, 04, 05 | **5 / 5 PASS** |
| **Raspberry Pi Cortex-M0+** | `rp2040:rp2040:rpipico` | `6.0.0` | 01, 02, 03, 04, 05 | **5 / 5 PASS** |
| **Total** | **3 Diverse Architectures** | — | **15 Cases** | **15 / 15 PASS** |

### Linkage Observations
- Zero undefined symbol references (`undefined reference to ...`).
- Zero multiple-definition conflicts across C standard libraries, math libraries, or Arduino core symbols.
- Zero truncated-relocation or memory segment overflow issues.
- On bare STM32 boards without an active SD or LittleFS driver, examples 02 and 03 correctly compile their graceful fallback diagnostic branch without linker failures.

---

## 3. PlatformIO Environment Matrix

Tested with **PlatformIO Core 6.1+** using `ci/platformio.ini`:

| Environment | Framework | Platform / Core | Selected Archive Profile | Result |
|---|---|---|---|:---:|
| `esp32s3` | Arduino | Espressif 32 (v2.0.17 legacy) | `esp32s3-2.0.17` (Xtensa LX7) | **PASS** |
| `stm32f4` | Arduino | ST STM32 (STM32duino 3.0.0) | `cortex-m4` (ARM Cortex-M4 hard-float) | **PASS** |
| `rp2040` | Arduino | Raspberry Pi Pico (Arduino-Pico 6.0.0) | `cortex-m0plus` (ARM Cortex-M0+) | **PASS** |

### Linker Script & Extra Script (`extras/platformio.py`)
- PlatformIO correctly invokes `extras/platformio.py` before building targets.
- Dynamic environment detection successfully matches the target MCU (`board_build.mcu`), core version, and FPU configuration to the appropriate `libesp_isp.a` precompiled archive.
- Flash and RAM footprints are verified within MCU hardware limits.

---

## 4. Host-Based Protocol & State Machine Simulation

Simulated against realistic Espressif ROM and flasher-stub protocol models:

| Test Group | Scenarios Tested | Status |
|---|---|:---:|
| **Reset & Strap Sequencing** | Inverted/non-inverted `BOOT` and `EN` polarities, multi-pin strapping assertions (`MTDO`, `GPIO8`), timing delays. | **PASS** |
| **SLIP Framing & Synchronization** | UART buffer wrapping, timeout recovery, byte-unstuffing (`0xDB 0xDC`, `0xDB 0xDD`), sync packet retransmissions. | **PASS** |
| **ROM & Stub Handshake** | Initial 115200 baud handshake, stub binary injection, RAM execution jump, dynamic baud switching to 921600. | **PASS** |
| **Hardware ID & Geometry** | Chip ID interrogation, MAC address readout, SPI flash manufacture/device ID, flash size calculation. | **PASS** |
| **Data Streaming & Checksums** | PROGMEM memory feeds, fragmented `Stream` buffers, 32-bit `0xFF` alignment padding, per-block MD5 checksum calculation. | **PASS** |
| **Negative Testing & Guardrails** | Corrupted block digests, premature stream termination, chip mismatch protection, boundary overflow rejection. | **PASS** |

---

## 5. Artifact & Supply Chain Integrity

To guarantee clean, secure, and reproducible open-source releases:

### Archive Manifest Verification (`docs/binary-manifest.json`)
All **13 precompiled static archives (`libesp_isp.a`)** match their cryptographically signed SHA-256 digests:
- ESP32 (Xtensa Dual-Core)
- ESP32-S2 (Xtensa Single-Core)
- ESP32-S3 (Xtensa Dual-Core, Core 3.x)
- ESP32-S3 (Xtensa Dual-Core, Core 2.0.17)
- ESP32-C2 (RISC-V RV32IMC)
- ESP32-C3 (RISC-V RV32IMC)
- ESP32-C5 (RISC-V RV32IMC)
- ESP32-C6 (RISC-V RV32IMAC)
- ESP32-P4 (RISC-V Dual-Core HP + LP)
- Cortex-M0+ (ARMv6-M, soft-float, RP2040)
- Cortex-M4 (ARMv7E-M, hard-float FPv4-SP-D16, STM32F4/G4)
- Cortex-M7 (ARMv7E-M, hard-float FPv5-D16/SP, STM32F7/H7)

### Upstream Alignment & Bundled Firmware
- **Espressif Original Checkout:** 283/283 source files verified identical to upstream `esp-serial-flasher` release tags. Zero unauthorized modifications to upstream source trees.
- **ESP32-C6 Demo Binaries:** `bootloader.bin`, `partition-table.bin`, and `esp_flasher_demo.bin` are hash-verified against `firmware/demo_c6/manifest.json`.
- **Embedded C Arrays:** Checked in `examples/*/DemoC6Firmware.h` byte-for-byte against the disk binaries.

### Independent ZIP Installation Test (`tools/validate_zip.py`)
1. Generates release archive `dist/ESP-ISP-1.0.0.zip` via deterministic packaging (`package_release.py`).
2. Creates an isolated Arduino sketchbook directory.
3. Installs `ESP-ISP-1.0.0.zip` using `arduino-cli lib install --zip-path`.
4. Compiles the installed `01_ISP_QuickStart_PROGMEM` example from scratch.
5. Verifies installed files match the ZIP contents exactly. Output recorded in `dist/ZIP-VALIDATION.json`.

---

## 6. How to Reproduce Validation Locally

Run the following commands from the repository root:

```bash
# 1. Run full 15-case Arduino CLI compilation suite
python tools/validate_compile.py --resume

# 2. Run 3-environment PlatformIO build
pio run --project-dir ci

# 3. Build release package and verify hashes
python tools/package_release.py

# 4. Verify ZIP package installation and compilation
python tools/validate_zip.py
```

---

## 7. Báo Cáo Kiểm Chứng (Tóm Tắt Tiếng Việt)

- **Biên dịch Arduino CLI:** 15/15 trường hợp kiểm thử trên 3 dòng vi điều khiển (ESP32-S3, STM32F446RE, RP2040) cho toàn bộ 5 ví dụ đều **PASS 100%**, không xảy ra xung đột thư viện hay thiếu symbol.
- **Biên dịch PlatformIO:** Đạt chuẩn trên cả 3 môi trường phần cứng, tự động nạp đúng thư viện tĩnh `libesp_isp.a` theo kiến trúc chip.
- **Giả lập giao thức:** Đã kiểm chứng toàn bộ quy trình Reset/BOOT, đồng bộ SLIP, bắt tay nạp stub, đổi baud rate tốc độ cao và kiểm tra mã MD5 từng khối flash.
- **Toàn vẹn gói phân phối:** Kiểm tra SHA-256 của toàn bộ 13 file archive, 3 file firmware demo và mã nhúng PROGMEM. File ZIP cài đặt độc lập thành công trên Arduino CLI.
