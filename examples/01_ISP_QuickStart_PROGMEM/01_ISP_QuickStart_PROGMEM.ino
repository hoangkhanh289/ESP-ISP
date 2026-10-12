#include <ESP_ISP.h>
#include "ISPBoard.h"
#include "DemoC6Firmware.h"
// Real, complete C6 bootloader + partition table + application, built from bundled source.
void setup() {
  Serial.begin(115200); delay(1500);
  auto config=ispConfig();
  EspISP.onProgress(ispProgress);
  if (!EspISP.begin(config) || !EspISP.connect(ISP_TARGET_ESP32C6)) { Serial.println(EspISP.getLastError()); return; }
  Serial.println(EspISP.getTargetName());
  for (const auto &part:demo_images) {
    if (!EspISP.flashPROGMEM(part.offset,part.data,part.size) ||
        !EspISP.verifyMD5(part.offset,part.size,part.md5)) {
      Serial.println(EspISP.getLastError()); return;
    }
  }
  EspISP.resetTarget(); // Only after every image passed MD5.
  Serial.println(EspISP.getLastError());
}
void loop() { delay(1000); }
