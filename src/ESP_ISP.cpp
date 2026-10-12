#include "ESP_ISP.h"
#if defined(ARDUINO_ARCH_ESP32)
#include <esp_arduino_version.h>
#if defined(ESP_ISP_ABI_CORE2)
#if ESP_ARDUINO_VERSION != ESP_ARDUINO_VERSION_VAL(2,0,17) || !defined(CONFIG_IDF_TARGET_ESP32S3)
#error "ESP-ISP legacy profile requires Arduino-ESP32 2.0.17 on ESP32-S3."
#endif
#elif ESP_ARDUINO_VERSION_MAJOR != 3
#error "ESP-ISP requires Arduino-ESP32 major 3, or the explicit legacy S3 profile."
#elif ESP_ARDUINO_VERSION != ESP_ARDUINO_VERSION_VAL(3,3,12)
#warning "ESP-ISP was built with Arduino-ESP32 3.3.12; this 3.x SDK ABI is not yet validated."
#endif
#endif
#if defined(ARDUINO_ARCH_STM32)
#if !defined(__ARM_PCS_VFP)
#error "ESP-ISP STM32 archives require the documented hard-float ABI."
#endif
#if defined(STM32F4xx)
#define ISP_ENTRY esp_isp_v1_f4
#elif defined(STM32G4xx)
#define ISP_ENTRY esp_isp_v1_g4
#elif defined(STM32F7xx)
#define ISP_ENTRY esp_isp_v1_f7
#elif defined(STM32H7xx)
#define ISP_ENTRY esp_isp_v1_h7
#else
#error "ESP-ISP has no binary profile for this STM32 family."
#endif
#elif defined(ARDUINO_ARCH_RP2040)
#include <RP2040Version.h>
#if ARDUINO_PICO_MAJOR != 6
#error "ESP-ISP RP2040 archives require Arduino-Pico major 6."
#elif ARDUINO_PICO_MINOR != 0 || ARDUINO_PICO_REVISION != 0
#warning "ESP-ISP was built with Arduino-Pico 6.0.0; this 6.x SDK ABI is not yet validated."
#endif
#if defined(PICO_RP2350) && PICO_RP2350
#error "Use the RP2040 profile; an RP2350 binary is not included."
#endif
#define ISP_ENTRY esp_isp_v1_rp2040
#else
#define ISP_ENTRY esp_isp_v1
#endif
extern "C" intptr_t ISP_ENTRY(uint32_t op,void *context,void *argument);
namespace {
struct ConnectArgs { int target;uint32_t baud; };
struct FlashArgs { uint32_t address;const uint8_t *data;Stream *stream;size_t size; };
struct DigestArgs { uint32_t address;size_t size;const void *expected; };
struct ProgressArgs { ISPProgressCallback callback; };
}
EspISPClass EspISP;
EspISPClass::EspISPClass():_ctx(nullptr),_local_error(ISP_SUCCESS) {}
EspISPClass::~EspISPClass() { if (_ctx) ISP_ENTRY(1,_ctx,nullptr); }
bool EspISPClass::context() {
    if (!_ctx) _ctx=reinterpret_cast<isp_internal_ctx_t*>(ISP_ENTRY(0,nullptr,nullptr));
    if (!_ctx) _local_error=ISP_ERROR_FAIL;
    return _ctx!=nullptr;
}
bool EspISPClass::begin(const EspISPConfig &config) { return context() && ISP_ENTRY(2,_ctx,const_cast<EspISPConfig*>(&config))!=0; }
void EspISPClass::end() { if (_ctx) ISP_ENTRY(3,_ctx,nullptr); }
bool EspISPClass::enterBootloader() { if (!context()) return false;return ISP_ENTRY(4,_ctx,nullptr)!=0; }
bool EspISPClass::connect(isp_target_chip_t target,uint32_t baud) { if (!context()) return false;ConnectArgs a={int(target),baud};return ISP_ENTRY(5,_ctx,&a)!=0; }
bool EspISPClass::isConnected() const { return _ctx && ISP_ENTRY(6,_ctx,nullptr)!=0; }
isp_target_chip_t EspISPClass::getTarget() const { return _ctx?isp_target_chip_t(ISP_ENTRY(7,_ctx,nullptr)):ISP_TARGET_AUTO; }
const char *EspISPClass::getTargetName() const {
    static const char *const names[]={"Unknown","ESP8266","ESP32","ESP32-S2","ESP32-C3","ESP32-S3","ESP32-C2","ESP32-C6","ESP32-H2","ESP32-C5","ESP32-P4"};
    unsigned target=unsigned(getTarget());return target<sizeof(names)/sizeof(names[0])?names[target]:names[0];
}
uint32_t EspISPClass::getFlashSize() { return context()?uint32_t(ISP_ENTRY(8,_ctx,nullptr)):0; }
uint32_t EspISPClass::getBaudRate() const { return _ctx?uint32_t(ISP_ENTRY(9,_ctx,nullptr)):0; }
bool EspISPClass::getMacAddress(uint8_t mac[6]) { return context() && ISP_ENTRY(10,_ctx,mac)!=0; }
bool EspISPClass::flashBuffer(uint32_t address,const uint8_t *data,size_t size) { FlashArgs a={address,data,nullptr,size};return context() && ISP_ENTRY(11,_ctx,&a)!=0; }
bool EspISPClass::flashPROGMEM(uint32_t address,const uint8_t *data,size_t size) { FlashArgs a={address,data,nullptr,size};return context() && ISP_ENTRY(12,_ctx,&a)!=0; }
bool EspISPClass::flashStream(uint32_t address,Stream &stream,size_t size) { FlashArgs a={address,nullptr,&stream,size};return context() && ISP_ENTRY(13,_ctx,&a)!=0; }
bool EspISPClass::verifyMD5(uint32_t address,size_t size,const uint8_t digest[16]) { DigestArgs a={address,size,digest};return context() && ISP_ENTRY(14,_ctx,&a)!=0; }
bool EspISPClass::verifyMD5Hex(uint32_t address,size_t size,const char hex[33]) { DigestArgs a={address,size,hex};return context() && ISP_ENTRY(15,_ctx,&a)!=0; }
void EspISPClass::resetTarget() { if (context()) ISP_ENTRY(16,_ctx,nullptr); }
void EspISPClass::onProgress(ISPProgressCallback callback) { if (context()) { ProgressArgs a={callback};ISP_ENTRY(17,_ctx,&a); } }
isp_error_t EspISPClass::lastErrorCode() const { return _ctx?isp_error_t(ISP_ENTRY(18,_ctx,nullptr)):_local_error; }
const char *EspISPClass::getLastError() const {
    switch (lastErrorCode()) {
    case ISP_SUCCESS:return "Success";case ISP_ERROR_FAIL:return "Operation or allocation failed";
    case ISP_ERROR_TIMEOUT:return "UART/Stream timeout";case ISP_ERROR_IMAGE_SIZE:return "Image exceeds flash capacity";
    case ISP_ERROR_INVALID_MD5:return "MD5 mismatch";case ISP_ERROR_INVALID_PARAM:return "Invalid configuration, state or argument";
    case ISP_ERROR_INVALID_TARGET:return "Unexpected target chip";case ISP_ERROR_UNSUPPORTED_CHIP:return "Chip/flash ID not recognized";
    case ISP_ERROR_UNSUPPORTED_FUNC:return "Unsupported function";case ISP_ERROR_INVALID_RESPONSE:return "Invalid ROM/stub response or SYNC failure";
    case ISP_ERROR_EFUSE_BLOCK_IN_USE:return "eFuse block protected/in use";case ISP_ERROR_EFUSE_BURN_FAILED:return "eFuse verification failed";
    default:return "Unknown ISP error";
    }
}
int8_t EspISPClass::slaveBootGPIO(isp_target_chip_t target) {
    switch(target) {
    case ISP_TARGET_ESP8266:case ISP_TARGET_ESP32:case ISP_TARGET_ESP32S2:case ISP_TARGET_ESP32S3:return 0;
    case ISP_TARGET_ESP32C2:case ISP_TARGET_ESP32C3:case ISP_TARGET_ESP32C6:case ISP_TARGET_ESP32H2:return 9;
    case ISP_TARGET_ESP32C5:return 28;case ISP_TARGET_ESP32P4:return 35;default:return -1;
    }
}
uint32_t EspISPClass::slaveBootloaderOffset(isp_target_chip_t target) {
    return target==ISP_TARGET_ESP32?0x1000:(target==ISP_TARGET_ESP32C5 || target==ISP_TARGET_ESP32P4?0x2000:0);
}
