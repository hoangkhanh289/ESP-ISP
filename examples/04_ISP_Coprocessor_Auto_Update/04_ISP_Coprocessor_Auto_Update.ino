#include <ESP_ISP.h>
#include "ISPBoard.h"
#include "DemoC6Firmware.h"
const char REQUIRED_VERSION[]="ESPFLASH-DEMO/1.0.0";
// Application-version exchange belongs to the SLAVE APP, not the ROM flasher.
// The bundled demo implements VERSION? at 115200 on the same UART0 pins.
bool applicationVersion(EspISPSerial &uart,const EspISPConfig &c,char *out,size_t capacity) {
#if defined(ARDUINO_ARCH_ESP32)
  uart.begin(115200,SERIAL_8N1,c.rx_pin,c.tx_pin);
#elif defined(ARDUINO_ARCH_STM32)
  uart.setRx((uint32_t)c.rx_pin); uart.setTx((uint32_t)c.tx_pin); uart.begin(115200,SERIAL_8N1);
#elif defined(ARDUINO_ARCH_RP2040)
  uart.setRX(c.rx_pin); uart.setTX(c.tx_pin); uart.begin(115200,SERIAL_8N1);
#else
  (void)c; uart.begin(115200,SERIAL_8N1);
#endif
  delay(300); while (uart.available()) uart.read();
  uart.print("VERSION?\n");
  size_t used=0; uint32_t start=millis(); bool complete=false;
  while ((uint32_t)(millis()-start)<1500) {
    int value=uart.read();
    if (value<0) { yield(); continue; }
    if (value=='\n') { complete=true; break; }
    if (value!='\r') { if (used+1>=capacity) break; out[used++]=(char)value; }
  }
  out[used]=0; uart.end(); return complete;
}
void setup() {
  Serial.begin(115200); delay(1500);
  auto config=ispConfig(); char version[48];
  if (!config.serial) { Serial.println("Select a separate physical slave UART in ISPBoard.h."); return; }
  if (applicationVersion(*config.serial,config,version,sizeof(version)) && strcmp(version,REQUIRED_VERSION)==0) {
    Serial.println("Version matches; no reset or flash performed."); return;
  }
  // This example is explicitly a provision/update policy for the bundled C6 demo.
  // Adapt the query and compatibility policy for an ESP-NIC or your own firmware.
  if (!EspISP.begin(config) || !EspISP.connect(ISP_TARGET_ESP32C6)) { Serial.println(EspISP.getLastError()); return; }
  EspISP.onProgress(ispProgress);
  for (const auto &part:demo_images)
    if (!EspISP.flashPROGMEM(part.offset,part.data,part.size)) { Serial.println(EspISP.getLastError()); return; }
  EspISP.resetTarget();
  // begin() owns this UART until end(); release it before the application query.
  EspISP.end(); delay(400);
  if (applicationVersion(*config.serial,config,version,sizeof(version)) && strcmp(version,REQUIRED_VERSION)==0)
    Serial.println("Update verified by the slave application.");
  else Serial.println("Flash MD5 passed, but the application did not confirm its version.");
}
void loop() { delay(1000); }
