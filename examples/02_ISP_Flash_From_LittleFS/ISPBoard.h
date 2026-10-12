#pragma once
#include <ESP_ISP.h>
// These are MASTER pins. Default examples target C6: ROM UART0 TX16/RX17, BOOT9.
#if defined(ARDUINO_ARCH_STM32)
static EspISPSerial slaveUART(PA10,PA9); // No dependency on enabling global Serial1.
#endif
inline EspISPConfig ispConfig() {
  EspISPConfig c;
#if defined(ARDUINO_ARCH_ESP32)
  c.serial=&Serial1;
#if defined(CONFIG_IDF_TARGET_ESP32P4)
  c.rx_pin=25; c.tx_pin=24; c.reset_pin=26; c.boot_pin=27;
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
  c.rx_pin=18; c.tx_pin=17; c.reset_pin=5; c.boot_pin=4;
#elif defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32C6)
  c.rx_pin=4; c.tx_pin=5; c.reset_pin=6; c.boot_pin=7;
#elif defined(CONFIG_IDF_TARGET_ESP32C5)
  c.rx_pin=6; c.tx_pin=7; c.reset_pin=10; c.boot_pin=11;
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
  c.rx_pin=16; c.tx_pin=17; c.reset_pin=18; c.boot_pin=21;
#else
  c.rx_pin=16; c.tx_pin=17; c.reset_pin=4; c.boot_pin=5;
#endif
#elif defined(ARDUINO_ARCH_STM32)
  c.serial=&slaveUART; c.rx_pin=PA10; c.tx_pin=PA9; c.reset_pin=PB0; c.boot_pin=PB1;
#elif defined(ARDUINO_ARCH_RP2040)
  c.serial=&Serial1; c.rx_pin=1; c.tx_pin=0; c.reset_pin=2; c.boot_pin=3;
#else
  // Fixed-pin UART cores: leave RX/TX=-1, select the board's physical UART.
  c.flash_baud=115200;
#if defined(SERIAL_PORT_HARDWARE)
  c.serial=&SERIAL_PORT_HARDWARE;
#elif defined(HAVE_HWSERIAL1)
  c.serial=&Serial1;
#endif
  c.reset_pin=2; c.boot_pin=3;
#endif
  return c;
}
inline void ispProgress(size_t written,size_t total) {
  Serial.print((unsigned)((uint64_t)written*100/total)); Serial.println("%");
}
