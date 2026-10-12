#pragma once
#include <Arduino.h>
#include "esp_isp_config.h"
#if defined(ARDUINO_ARCH_STM32) && __has_include(<Serial.h>)
#include <Serial.h>
using EspISPSerial=Uart;
#elif defined(ARDUINO_ARCH_RP2040) && __has_include(<SerialUART.h>)
#include <SerialUART.h>
using EspISPSerial=SerialUART;
#else
using EspISPSerial=HardwareSerial;
#endif
enum isp_target_chip_t {
    ISP_TARGET_AUTO=0,ISP_TARGET_ESP8266,ISP_TARGET_ESP32,ISP_TARGET_ESP32S2,
    ISP_TARGET_ESP32C3,ISP_TARGET_ESP32S3,ISP_TARGET_ESP32C2,ISP_TARGET_ESP32C6,
    ISP_TARGET_ESP32H2,ISP_TARGET_ESP32C5,ISP_TARGET_ESP32P4
};
enum isp_error_t {
    ISP_SUCCESS=0,ISP_ERROR_FAIL=1,ISP_ERROR_TIMEOUT=2,ISP_ERROR_IMAGE_SIZE=3,
    ISP_ERROR_INVALID_MD5=4,ISP_ERROR_INVALID_PARAM=5,ISP_ERROR_INVALID_TARGET=6,
    ISP_ERROR_UNSUPPORTED_CHIP=7,ISP_ERROR_UNSUPPORTED_FUNC=8,ISP_ERROR_INVALID_RESPONSE=9,
    ISP_ERROR_EFUSE_BLOCK_IN_USE=10,ISP_ERROR_EFUSE_BURN_FAILED=11,
    ISP_ERROR_CHIP_NOT_RECOGNIZED=ISP_ERROR_UNSUPPORTED_CHIP
};
struct EspISPConfig {
    int32_t rx_pin=-1,tx_pin=-1;
    uint32_t init_baud=ESP_ISP_INIT_BAUD,flash_baud=ESP_ISP_FLASH_BAUD;
    int32_t reset_pin=-1,boot_pin=-1;
    uint32_t reset_hold_ms=ESP_ISP_RESET_HOLD_MS,boot_hold_ms=ESP_ISP_BOOT_HOLD_MS;
    bool invert_reset=false,invert_boot=false;
    bool release_reset=true,release_boot=true;
    int32_t strap_high_pin=-1;
    bool release_strap_high=true;
    EspISPSerial *serial=nullptr;
    bool use_native_uart=true;
    uint8_t uart_port=1;
    bool use_stub=true,verify_after_write=true;
    uint32_t stream_timeout_ms=ESP_ISP_STREAM_TIMEOUT_MS,sync_timeout_ms=100,sync_trials=10;
};
typedef void (*ISPProgressCallback)(size_t written,size_t total);
