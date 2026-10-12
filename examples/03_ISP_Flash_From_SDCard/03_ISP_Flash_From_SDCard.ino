#include <ESP_ISP.h>
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_RP2040) || __has_include(<SD.h>)
#include <SPI.h>
#include <SD.h>
#include "ISPBoard.h"
// Change target and offsets together using your build's flasher_args.json.
constexpr isp_target_chip_t SLAVE=ISP_TARGET_ESP32C6;
struct Image { const char *file; uint32_t address; };
Image images[]={{"/bootloader.bin",EspISPClass::slaveBootloaderOffset(SLAVE)},
  {"/partition-table.bin",0x8000},{"/app.bin",0x10000}};
#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_IDF_TARGET_ESP32S3)
constexpr int SD_CS=10;
#elif defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_IDF_TARGET_ESP32P4)
constexpr int SD_CS=21;
#elif defined(ARDUINO_ARCH_ESP32)
constexpr int SD_CS=13;
#else
constexpr int SD_CS=10;
#endif
void setup() {
  Serial.begin(115200); delay(1500);
#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_IDF_TARGET_ESP32S3)
  SPI.begin(12,13,11,SD_CS);
#elif defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_IDF_TARGET_ESP32P4)
  SPI.begin(18,20,19,SD_CS);
#endif
  if (!SD.begin(SD_CS)) { Serial.println("SD mount failed"); return; }
  // Preflight all files before any slave erase/write.
  for (const auto &image:images) {
    File f=SD.open(image.file,FILE_READ);
    if (!f || !f.size()) { Serial.print("Missing image: "); Serial.println(image.file); return; }
    f.close();
  }
  auto config=ispConfig(); EspISP.onProgress(ispProgress);
  if (!EspISP.begin(config) || !EspISP.connect(SLAVE)) { Serial.println(EspISP.getLastError()); return; }
  for (const auto &image:images) {
    File f=SD.open(image.file,FILE_READ);
    if (!f || !EspISP.flashStream(image.address,f,f.size())) { Serial.println(EspISP.getLastError()); return; }
    f.close();
  }
  // Add ota_data_initial.bin at its manifest offset when an OTA layout requires it.
  EspISP.resetTarget(); Serial.println("All images verified; slave reset.");
}
void loop() { delay(1000); }

#else
void setup() { Serial.begin(115200); delay(1500); Serial.println("Install an SD library compatible with this core to enable this example."); }
void loop() { delay(1000); }
#endif
