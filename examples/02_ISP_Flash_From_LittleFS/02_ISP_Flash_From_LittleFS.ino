#include <ESP_ISP.h>
#include "ISPBoard.h"
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_RP2040) || __has_include(<LittleFS.h>)
#include <LittleFS.h>
void setup() {
  Serial.begin(115200); delay(1500);
  // Does not format or erase your master filesystem. Copy a matching app image first.
  if (!LittleFS.begin()) { Serial.println("LittleFS mount failed"); return; }
  File firmware=LittleFS.open("/firmware.bin","r");
  if (!firmware || !firmware.size()) { Serial.println("Missing /firmware.bin"); return; }
  auto config=ispConfig(); EspISP.onProgress(ispProgress);
  if (!EspISP.begin(config) || !EspISP.connect(ISP_TARGET_ESP32C6) ||
      !EspISP.flashStream(0x10000,firmware,firmware.size())) { Serial.println(EspISP.getLastError()); return; }
  firmware.close(); EspISP.resetTarget();
  Serial.println("Application updated; existing bootloader/partition layout must match.");
}
#else
void setup() { Serial.begin(115200); Serial.println("This core has no built-in LittleFS. Use the SD example or another Stream."); }
#endif
void loop() { delay(1000); }
