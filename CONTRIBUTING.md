# Contributing to ESP-ISP

Thank you for your interest in contributing to **ESP-ISP**! We welcome contributions from the embedded systems and open-source community, including bug reports, documentation enhancements, feature proposals, and hardware test reports.

---

## Code of Conduct

We expect all contributors to adhere to open, respectful, and constructive communication. Please treat everyone with respect and focus on technical merits, reproducible evidence, and collaborative problem-solving.

---

## Repository Architecture & Scope

To keep our build reproducible across diverse embedded toolchains, the repository is structured into distinct layers:

```
ESP-ISP (Public Consumer Repository)
 ├── src/                      # Public C++ Facade & API headers
 ├── extras/                   # PlatformIO build script & archive selector
 ├── examples/                 # 5 standalone Arduino IDE / PlatformIO examples
 ├── docs/                     # Technical specs, validation reports, licenses
 ├── firmware/demo_c6/         # Pre-verified ESP32-C6 test firmware
 └── tools/                    # Local validation, packaging, and integrity scripts
```

- **Public C++ Facade (`src/`)**: High-level C++ classes (`ESP_ISP`, `esp_isp_config.h`, `esp_isp_types.h`) exposing a clean Arduino/PlatformIO API.
- **Precompiled Static Archives (`libesp_isp.a`)**: Optimized flasher engines built for specific CPU/FPU architectures (Xtensa LX6/LX7, RISC-V RV32IMC, ARM Cortex-M0+/M4F/M7F).
- **Core Flashing Pipeline**: The underlying flasher engine source code is maintained and built via the upstream pipeline at [esp-serial-flasher/tools/esp_isp](https://github.com/hoangkhanh289/esp-serial-flasher/tree/main/tools/esp_isp). Do not submit PRs adding private loader or raw stub C sources directly to this repository.

---

## Local Development & Verification

Before submitting a Pull Request, please ensure all sketches compile cleanly and that packaging validation passes.

### Prerequisites

- **Python 3.9+** (Standard libraries only)
- **Arduino CLI** (`1.5.0+`) with the following board cores installed:
  - `esp32:esp32` (v3.x and legacy v2.0.17 for ESP32-S3)
  - `STMicroelectronics:stm32` (v3.0.0+)
  - `rp2040:rp2040` (v6.0.0+)
- **PlatformIO Core** (`pio 6.1+`)

### Validation Commands

Run the following checks from the root of the repository:

#### 1. Arduino CLI Compilation Checks

```bash
# Test ESP32-S3 Master
arduino-cli compile --fqbn esp32:esp32:esp32s3 --library . examples/01_ISP_QuickStart_PROGMEM
arduino-cli compile --fqbn esp32:esp32:esp32s3 --library . examples/05_ISP_Hardware_Wiring_Diagnostic

# Test STM32 Master (Nucleo-F446RE)
arduino-cli compile --fqbn STMicroelectronics:stm32:Nucleo_64:pnum=NUCLEO_F446RE --library . examples/05_ISP_Hardware_Wiring_Diagnostic

# Test RP2040 Master (Raspberry Pi Pico)
arduino-cli compile --fqbn rp2040:rp2040:rpipico --library . examples/05_ISP_Hardware_Wiring_Diagnostic
```

#### 2. PlatformIO Multi-Environment Build

```bash
pio run --project-dir ci -e esp32s3
pio run --project-dir ci -e stm32f4
pio run --project-dir ci -e rp2040
```

#### 3. Release Package & ZIP Integrity

```bash
# Verifies file allowlists, archive hashes, and builds the distribution ZIP
python tools/package_release.py

# Installs the built ZIP into an isolated sandbox and compiles the real demo
python tools/validate_zip.py
```

All commands must exit with code `0`.

---

## How to Contribute

### 1. Reporting Bugs

If you encounter unexpected behavior or failed flashing:
1. Search [Existing Issues](https://github.com/hoangkhanh289/ESP-ISP/issues) to avoid duplicates.
2. Open a new issue using our bug report template and include:
   - **Master Board:** Exact MCU (e.g., ESP32-S3 DevKitC-1, Raspberry Pi Pico, Nucleo-F446RE) and framework version.
   - **Target Slave:** Exact target chip (e.g., ESP32-C6-WROOM-1, ESP32-C3 mini).
   - **Hardware Setup:** Wiring details, pull-up resistor values on `EN`/`BOOT`, auxiliary strapping pins, and logic levels (3.3V).
   - **Baud Rate:** Flashing baud rate used (e.g., 115200, 460800, 921600).
   - **Console Logs:** Full Serial Monitor log with verbose debugging enabled (`setDebugLevel(ESP_ISP_LOG_DEBUG)`).

### 2. Submitting Hardware Test Reports

Physical hardware validation on new Master-Slave combinations is extremely valuable! When submitting test reports:
- Clearly state the Master MCU and Target Slave chip.
- Note whether 3.3V logic level shifters were required (if Master != 3.3V).
- Include the flash size, flashing duration, and verified MD5 checksum.
- Share your pin definition config.

### 3. Pull Requests (PRs)

1. **Fork** the repository and create a descriptive branch:
   ```bash
   git checkout -b feature/improved-diagnostic-output
   ```
2. **Make your changes** following our coding conventions:
   - Write clean, standards-compliant C++11.
   - Maintain public header backward compatibility.
   - Never add raw binary blobs or edit `.a` archives directly without updating `binary-manifest.json` and upstream tooling.
   - Keep memory allocations explicit and clean; do not use heavy dynamic allocations in critical flashing loops.
3. **Run all validation tests** outlined above.
4. **Commit** with clear, semantic commit messages (e.g., `fix(diagnostic): clarify strapping pin error message`).
5. **Push** to your fork and submit a PR against `main`.

---

## Hướng Dẫn Đóng Góp (Tiếng Việt)

1. **Kiểm tra trước khi gửi PR:**
   - Biên dịch thành công các ví dụ qua `arduino-cli` và `pio run`.
   - Chạy `python tools/package_release.py` và `python tools/validate_zip.py` đảm bảo không có lỗi hash hoặc sai lệch cấu trúc file.
2. **Báo cáo lỗi phần cứng:**
   - Cung cấp sơ đồ nối dây (chân TX, RX, BOOT, EN, GND chung).
   - Ghi rõ giá trị trở kéo (pull-up 10k trên EN/BOOT) và log Serial khi bật debug.
3. **Phạm vi mã nguồn:**
   - Kho chứa này (`ESP-ISP`) là thư viện phân phối công khai chứa facade C++, ví dụ và file thư viện tĩnh `.a`.
   - Mã nguồn private flasher stub và engine nạp được quản lý và build từ repository upstream. Vui lòng không commit trực tiếp mã nguồn stub hoặc file nhị phân chưa được kiểm định.
