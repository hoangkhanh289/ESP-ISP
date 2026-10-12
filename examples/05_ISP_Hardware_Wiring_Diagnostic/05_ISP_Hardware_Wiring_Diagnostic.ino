#include <ESP_ISP.h>
#include "ISPBoard.h"
void setup() {
  Serial.begin(115200); delay(1500);
  auto config=ispConfig();
  config.use_stub=false; config.flash_baud=115200; // Measure forced pins + ROM SYNC + detection only.
  if (!EspISP.begin(config)) { Serial.println(EspISP.getLastError()); return; }
  uint32_t start=micros();
  bool connected=EspISP.connect(ISP_TARGET_AUTO,115200);
  Serial.print("Reset hold ms: "); Serial.println(config.reset_hold_ms);
  Serial.print("Boot hold ms: "); Serial.println(config.boot_hold_ms);
  Serial.print("Pulse + SYNC + detection microseconds: "); Serial.println((uint32_t)(micros()-start));
  Serial.println(EspISP.getLastError());
  if (connected) { Serial.println(EspISP.getTargetName()); Serial.println(EspISP.getFlashSize()); }
  // Does not write, erase or burn eFuses. Reset runs the existing flash image.
  EspISP.resetTarget();
}
void loop() { delay(1000); }
