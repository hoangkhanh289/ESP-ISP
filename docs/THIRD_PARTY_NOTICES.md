# Third-Party Notices and Disclosures

This distribution of **ESP-ISP** incorporates third-party open-source software components under various permissive licenses. This document provides proper attribution and summarizes the governing licenses as required by their respective copyright holders.

---

## 1. Espressif Systems Components

### `esp-serial-flasher` (v2.1.0)
- **Copyright:** Copyright (c) 2020-2024 Espressif Systems (Shanghai) CO., LTD.
- **License:** [Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0)
- **Description:** Flasher protocol logic, serial communication layers, and chip-specific target configurations.
- **Upstream Repository:** [https://github.com/espressif/esp-serial-flasher](https://github.com/espressif/esp-serial-flasher)

### `esp-flasher-stub` (v1.3.0)
- **Copyright:** Copyright (c) Espressif Systems (Shanghai) CO., LTD.
- **License:** Dual-licensed under [Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0) OR [MIT License](https://opensource.org/licenses/MIT). This distribution adopts the **Apache License 2.0**.
- **Description:** Precompiled RAM execution stubs injected into target slave MCUs to achieve high-speed SPI flash programming and on-chip MD5 calculation.

---

## 2. Cryptographic Digest Components

### MD5 Message-Digest Implementation
The MD5 hash verification engine utilizes Colin Plumb and Jouni Malinen's public-domain and BSD-licensed implementation:

```text
/*
 * MD5 hash implementation and interface functions
 * Copyright (c) 2003-2005, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */
/*
 * This code implements the MD5 message-digest algorithm.
 * The algorithm is due to Ron Rivest.  This code was
 * written by Colin Plumb in 1993, no copyright is claimed.
 * This code is in the public domain; do with it what you wish.
 *
 * Equivalent code is available from RSA Data Security, Inc.
 * This code has been tested against that, and is equivalent,
 * except that you don't need to include two pages of legalese
 * with every copy.
 *
 * To compute the message digest of a chunk of bytes, declare an
 * MD5Context structure, pass it to MD5Init, call MD5Update as
 * needed on buffers full of bytes, and then call MD5Final, which
 * will fill a supplied 16-byte array with the digest.
 */
```

- **License Text:** See [MD5_BSD_LICENSE.txt](MD5_BSD_LICENSE.txt) for full BSD terms.

---

## 3. Bundled Demo Firmware Runtime Notices

The precompiled ESP32-C6 test images located in `firmware/demo_c6/` (`bootloader.bin`, `partition-table.bin`, `esp_flasher_demo.bin`) were compiled using the **Espressif IoT Development Framework (ESP-IDF v5.5.5)**.

These binary images contain statically compiled runtime components provided by third parties, including:
- **FreeRTOS kernel:** MIT License
- **Newlib C Standard Library:** Various permissive BSD/MIT-style licenses
- **Espressif HAL & Driver Components:** Apache License 2.0

Full license texts corresponding to all components included in the demo firmware are maintained under the [`firmware/licenses/`](../firmware/licenses/) directory.

---

## 4. Ghi Nhận Thành Phần Bên Thứ Ba (Tiếng Việt)

- Thư viện **ESP-ISP** được phát hành theo giấy phép **Apache-2.0**.
- Các thành phần lõi giao thức nạp nối tiếp kế thừa từ **esp-serial-flasher** và **esp-flasher-stub** của Espressif Systems (Apache-2.0).
- Thuật toán băm kiểm tra tính toàn vẹn MD5 sử dụng mã nguồn chuẩn của Colin Plumb và Jouni Malinen (BSD License).
- Các file firmware mẫu ESP32-C6 được build từ ESP-IDF chứa runtime của FreeRTOS (MIT) và Newlib; chi tiết bản quyền được lưu trữ đầy đủ tại `firmware/licenses/`.
