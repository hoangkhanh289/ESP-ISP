# ESP-ISP: Thư viện Nạp In-System Tự Động Cho Vi Điều Khiển ESP

[![Tác giả](https://img.shields.io/badge/Tác%20giả-Hoàng%20Khánh-informational.svg)](https://github.com/hoangkhanh289)
[![GitHub Repo](https://img.shields.io/badge/GitHub-ESP--ISP-blue.svg)](https://github.com/hoangkhanh289/ESP-ISP)
[![Phiên bản](https://img.shields.io/badge/Phiên%20bản-v1.0.0-success.svg)](https://github.com/hoangkhanh289/ESP-ISP/releases)
[![Giấy phép](https://img.shields.io/badge/Giấy%20phép-Apache%202.0-green.svg)](LICENSE)
[![Arduino](https://img.shields.io/badge/Arduino-1.5%2B%20Precompiled-blue.svg)](https://github.com/arduino/Arduino/wiki/Arduino-IDE-1.5:-Library-specification)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-Tương%20thích-orange.svg)](extras/platformio.py)
[![Master hỗ trợ](https://img.shields.io/badge/Master-ESP32%20%7C%20STM32%20%7C%20RP2040-blueviolet.svg)](#1-danh-sách-master-mcu-hỗ-trợ)
[![Slave hỗ trợ](https://img.shields.io/badge/Slave-ESP32%20%7C%20C6%20%7C%20C5%20%7C%20C3%20%7C%20S3%20%7C%20P4%20%7C%208266-success.svg)](#2-danh-sách-chip-slave-hỗ-trợ-nạp)

> **Ngôn ngữ / Language:** [🇬🇧 English](README.md) | **Tiếng Việt**

---

## Giới thiệu & Tổng quan

**ESP-ISP (In-System Programmer)** là giải pháp phần mềm chuyên dụng dành cho hệ sinh thái Arduino và PlatformIO, cho phép vi điều khiển chủ (**Master MCU** như **ESP32-S3, ESP32-P4, STM32, RP2040**) đóng vai trò như một mạch nạp phần cứng thông minh, tự động nạp, cập nhật và cứu hộ firmware cho chip phụ (**Slave Coprocessor** như **ESP32-C6, C5, C3, S3, P4, ESP8266**) được hàn cố định trên bo mạch:

* ⚡ **Nạp Trực Tiếp Trên Mạch (True In-System Programming)**: Không cần tháo chip, không cần nút bấm thủ công và loại bỏ hoàn toàn các mạch chuyển đổi USB-UART rời (CP2102, CH340). Master tự động điều khiển xung `RESET / EN` và chân `BOOT / IO0` để đưa chip vào ROM Download Mode.
* 🚀 **Tốc độ cao & Tin cậy tuyệt đối**: Bắt đầu đồng bộ ROM ở tốc độ 115,200 bps, tự động nạp RAM stub flasher tối ưu, tăng tốc độ đường truyền lên **921,600 bps** (hoặc cao hơn) và xác minh tính toàn vẹn dữ liệu từng phân vùng bằng mã băm phần cứng MD5.
* 🔄 **Linh hoạt Nguồn Dữ liệu**: Hỗ trợ nạp firmware nhúng trực tiếp trong mảng bộ nhớ Flash (`PROGMEM`), đọc từ hệ thống tệp tin Flash (`LittleFS`), nạp từ `Thẻ nhớ MicroSD`, hoặc qua luồng dữ liệu mạng OTA.
* 📦 **Lõi Nhị Phân Biên Dịch Sẵn (Arduino 1.5+ Precompiled Engine)**: Toàn bộ thuật toán nạp phức tạp, state machine và bộ giải mã SLIP được đóng gói sẵn trong các thư viện tĩnh tối ưu (`libesp_isp.a`), giúp sketch của bạn biên dịch cực nhanh và không bị phình dung lượng mã nguồn.

---

## Kiến Trúc Hệ Thống

```text
+------------------------------------+                                   +------------------------------------+
|         MASTER MCU (HOST)          |                                   |       SLAVE ESP (COPROCESSOR)      |
|    ESP32-S3 / ESP32-P4 / STM32     |                                   |    ESP32-C6 / C5 / C3 / S3 / P4    |
|                                    |         Hardware ISP Bus          |                                    |
|  - Ứng dụng Arduino Sketch         | <===============================> |  - Hệ thống nhúng / Mạch ứng dụng  |
|  - #include <ESP_ISP.h>            |                                   |  - ROM Bootloader / Download Mode  |
|  - EspISP.begin(config)            |     UART TX --------> ROM RX      |  - Thực thi RAM Flasher Stub       |
|  - EspISP.connect(target, baud)    |     UART RX <-------- ROM TX      |  - Ghi SPI Flash & Phân vùng       |
|  - EspISP.flashStream(...)         |     reset_pin ------> EN / CHIP_PU|                                    |
|  - EspISP.verifyMD5(...)           |     boot_pin -------> BOOT Strap  |                                    |
|  - EspISP.resetTarget()            |     GND ------------> GND         |                                    |
+------------------------------------+                                   +------------------------------------+
```

---

## Bảng Tương Thích Phần Cứng

### 1. Danh sách Master MCU hỗ trợ

| Dòng Master MCU | Core tương thích | Giao diện UART Master | Thư mục thư viện tĩnh `src/` |
| :--- | :--- | :---: | :--- |
| **ESP32-P4** | Arduino-ESP32 3.x | Hardware UART gốc | `src/esp32p4/libesp_isp.a` |
| **ESP32-S3** | Arduino-ESP32 3.x / 2.0.17 | Hardware UART gốc | `src/esp32s3/libesp_isp.a` |
| **ESP32 Classic** | Arduino-ESP32 3.x | Hardware UART gốc | `src/esp32/libesp_isp.a` |
| **ESP32-C3 / C6 / C5** | Arduino-ESP32 3.x | Hardware UART gốc | `src/esp32c3/`, `c6/`, `c5/` |
| **STM32F4 / G4** | STM32duino 3.0+ | `Uart` / `HardwareSerial` | `src/cortex-m4/libesp_isp.a` |
| **STM32F7 / H7** | STM32duino 3.0+ | `Uart` / `HardwareSerial` | `src/cortex-m7/libesp_isp.a` |
| **RP2040 (Raspberry Pi Pico)** | Arduino-Pico (Earle Philhower) 6.x | `SerialUART` | `src/cortex-m0plus/libesp_isp.a` |

### 2. Danh sách Chip Slave hỗ trợ nạp

| Dòng Chip Slave | Chân BOOT chính | Yêu cầu mức logic chân Strap phụ | Offset Bootloader phổ biến |
| :--- | :---: | :--- | :---: |
| **ESP32-C6 / C3 / C2 / H2** | `GPIO 9` | `GPIO 8` giữ mức HIGH (treo trở lên 3.3V) | `0x0000` |
| **ESP32-C5** | **`GPIO 28`** | **`GPIO 27` giữ mức HIGH** | **`0x2000`** |
| **ESP32-P4** | **`GPIO 35`** | **`GPIO 36` giữ mức HIGH** | **`0x2000`** |
| **ESP32-S3 / S2** | `GPIO 0` | `GPIO 46` mức LOW hoặc thả nổi | `0x0000` |
| **ESP32 Classic** | `GPIO 0` | `GPIO 2` mức LOW hoặc thả nổi, `GPIO 12` LOW | `0x1000` |
| **ESP8266** | `GPIO 0` | `GPIO 2` HIGH, `GPIO 15` LOW | `0x0000` |

> [!IMPORTANT]
> **Điểm lưu ý phần cứng quan trọng**:
> * **ESP32-C5** dùng chân **GPIO 28** làm chân BOOT strap (khác với GPIO 9 của các dòng C-series khác).
> * **ESP32-P4** dùng chân **GPIO 35** làm chân BOOT strap.
> * Bắt buộc phải **nối chung Mass (Common GND)** giữa Master và Slave.

---

## Sơ Đồ Đấu Nối Phần Cứng

### Sơ đồ nguyên lý chuẩn (Khuyến nghị)

```text
               SLAVE 3.3V                   SLAVE 3.3V
                    |                            |
                  10 kΩ                        10 kΩ
                    |                            |
  Master RESET ---- EN (CHIP_PU)    BOOT Strap --+------- Master BOOT
                    |
              Tụ lọc (0.1 µF ~ 1 µF)
                    |
                   GND
```

### Bảng kết nối tín hiệu

| Tín hiệu | Chân Master (Ví dụ ESP32-S3) | Chân Slave (Ví dụ ESP32-C6) | Mô tả kỹ thuật |
| :--- | :---: | :---: | :--- |
| **TX → RX** | `GPIO 17` | `UART0 RX (GPIO 17)` | Master truyền lệnh và khối dữ liệu nạp |
| **RX ← TX** | `GPIO 18` | `UART0 TX (GPIO 16)` | Master nhận phản hồi đồng bộ và mã MD5 |
| **Reset** | `GPIO 5` | `CHIP_EN` | Kéo mức LOW để reset chip Slave |
| **Boot Strap** | `GPIO 4` | `BOOT (GPIO 9)` | Kéo LOW khi nhả Reset để vào Bootloader |
| **GND** | `GND` | `GND` | **Bắt buộc nối chung Mass** |

---

## Hướng Dẫn Sử Dụng Nhanh

### 1. Nạp Firmware từ Bộ Nhớ Flash (PROGMEM)

```cpp
#include <ESP_ISP.h>
#include "DemoC6Firmware.h" // Chứa mảng byte bootloader, partition-table, app

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
    Serial.printf("Tiến độ: %u%%\r", (unsigned)(written * 100 / total));
  });

  if (!EspISP.begin(cfg)) {
    Serial.printf("Khởi tạo ISP thất bại: %s\n", EspISP.getLastError());
    return;
  }

  Serial.println("Đang kết nối chip phụ...");
  if (!EspISP.connect(ISP_TARGET_ESP32C6)) {
    Serial.printf("Kết nối thất bại: %s\n", EspISP.getLastError());
    return;
  }

  Serial.printf("Đã kết nối: %s | Flash: %u MB\n", 
                EspISP.getTargetName(), 
                EspISP.getFlashSize() / (1024 * 1024));

  // Nạp lần lượt các phân vùng
  for (const auto &img : demo_images) {
    Serial.printf("Nạp phân vùng tại 0x%05X (%u bytes)...\n", img.offset, img.size);
    if (!EspISP.flashPROGMEM(img.offset, img.data, img.size) ||
        !EspISP.verifyMD5(img.offset, img.size, img.md5)) {
      Serial.printf("Lỗi nạp hoặc sai MD5: %s\n", EspISP.getLastError());
      return;
    }
  }

  Serial.println("\nNạp hoàn tất! Khởi động ứng dụng mới...");
  EspISP.resetTarget();
  EspISP.end();
}

void loop() {
  delay(1000);
}
```

### 2. Nạp Firmware từ LittleFS hoặc Thẻ Nhớ SD

```cpp
#include <ESP_ISP.h>
#include <LittleFS.h>

void flashFromFilesystem() {
  File file = LittleFS.open("/firmware.bin", "r");
  if (!file || file.size() == 0) {
    Serial.println("Không tìm thấy file firmware!");
    return;
  }

  // Nạp vào offset ứng dụng (ví dụ: 0x10000 cho App)
  if (EspISP.flashStream(0x10000, file, file.size())) {
    Serial.println("Cập nhật firmware thành công!");
    EspISP.resetTarget();
  } else {
    Serial.printf("Lỗi nạp: %s\n", EspISP.getLastError());
  }
  file.close();
}
```

---

## Danh Mục Ví Dụ Mẫu (Examples)

Tất cả các ví dụ hoàn chỉnh nằm trong thư mục [`examples/`](examples/):

| Thư mục ví dụ | Tính năng chính | Ứng dụng thực tế |
| :--- | :--- | :--- |
| [`01_ISP_QuickStart_PROGMEM`](examples/01_ISP_QuickStart_PROGMEM/) | **Nạp từ PROGMEM** | Mạch độc lập: Nhúng sẵn Bootloader, Partition Table và App trong mã nguồn Master để tự nạp. |
| [`02_ISP_Flash_From_LittleFS`](examples/02_ISP_Flash_From_LittleFS/) | **Nạp từ LittleFS** | Cập nhật firmware chip phụ từ file `.bin` lưu trong bộ nhớ Flash nội của Master. |
| [`03_ISP_Flash_From_SDCard`](examples/03_ISP_Flash_From_SDCard/) | **Nạp từ Thẻ nhớ SD** | Thiết bị nạp cầm tay: Cắm thẻ nhớ chứa các file `.bin` để nạp nhanh hàng loạt thiết bị ngoài hiện trường. |
| [`04_ISP_Coprocessor_Auto_Update`](examples/04_ISP_Coprocessor_Auto_Update/) | **Tự động Đồng bộ Phiên bản** | Đọc phiên bản của chip phụ khi khởi động, tự động nạp nâng cấp nếu phát hiện phiên bản cũ. |
| [`05_ISP_Hardware_Wiring_Diagnostic`](examples/05_ISP_Hardware_Wiring_Diagnostic/) | **Chẩn đoán Đấu dây Phần cứng** | Kiểm tra mức logic chân, đo thời gian phản hồi, đọc MAC Address và Flash ID mà không ghi đè dữ liệu. |

---

## Cài Đặt & Tích Hợp

### Trên Arduino IDE
1. Tải file `.zip` phát hành tại mục [Releases](https://github.com/hoangkhanh289/ESP-ISP/releases).
2. Mở Arduino IDE, chọn **Sketch → Include Library → Add .ZIP Library...** và chọn file vừa tải.
3. Mở ví dụ mẫu qua **File → Examples → ESP-ISP → 01_ISP_QuickStart_PROGMEM**.

### Trên PlatformIO
Thêm trực tiếp liên kết thư viện vào tệp `platformio.ini`:

```ini
[env:esp32s3]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino
lib_deps =
    https://github.com/hoangkhanh289/ESP-ISP.git
```

---

## Bảng Mã Lỗi & Chẩn Đoán Sự Cố

| Mã Lỗi | Nguyên nhân phổ biến | Giải pháp xử lý |
| :--- | :--- | :--- |
| `ISP_ERROR_TIMEOUT` (2) | Chưa nối chung GND, cắm ngược TX/RX, nguồn 3.3V không đủ dòng, hoặc thời gian giữ reset quá ngắn. | Nối chung mass GND; kiểm tra chiều Master TX $\rightarrow$ Slave RX; tăng `reset_hold_ms` lên 150–200 ms; gắn tụ lọc 0.1–1µF ở chân `EN`. |
| `ISP_ERROR_INVALID_MD5` (4) | Dữ liệu ghi bị lỗi do dây nối quá dài hoặc tốc độ baudrate quá cao so với chất lượng đường truyền. | Giảm `flash_baud` xuống 921600 hoặc 115200 bps; rút ngắn dây UART; gắn tụ lọc 100nF sát chân nguồn chip Slave. |
| `ISP_ERROR_UNSUPPORTED_CHIP` (7) | Chip phụ chưa vào được ROM Download Mode hoặc chân strap phụ bị sai mức logic. | Kiểm tra chân BOOT (GPIO9 cho C6/C3, GPIO28 cho C5, GPIO35 cho P4); đảm bảo chân strap phụ được kéo đúng mức. |
| `ISP_ERROR_IMAGE_SIZE` (3) | Kích thước file firmware vượt quá dung lượng bộ nhớ Flash thực tế của chip phụ. | Kiểm tra Flash ID qua hàm `getFlashSize()`; kiểm tra lại file `partition-table.bin`. |

---

## Tác Giả & Bản Quyền

* 👤 **Tác giả & Nhà phát triển**: **Hoàng Khánh** ([@hoangkhanh289](https://github.com/hoangkhanh289))
* 🌐 **Kho lưu trữ chính thức**: [https://github.com/hoangkhanh289/ESP-ISP](https://github.com/hoangkhanh289/ESP-ISP)
* 📄 **Giấy phép**: Phát hành theo chuẩn mã nguồn mở **Apache License 2.0**. Xem chi tiết tại [LICENSE](LICENSE) và [NOTICE](NOTICE).
* 🤝 **Ghi nhận nguồn mở**: Kế thừa và tương thích các thành phần từ Espressif Systems (`esp-serial-flasher`, `esp-flasher-stub`) và Colin Plumb / Jouni Malinen (MD5). Xem chi tiết tại [Third-Party Notices](docs/THIRD_PARTY_NOTICES.md).
