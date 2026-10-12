#pragma once
#include "esp_isp_types.h"
struct isp_internal_ctx_t;
class EspISPClass {
public:
    EspISPClass();
    ~EspISPClass();
    EspISPClass(const EspISPClass&)=delete;
    EspISPClass& operator=(const EspISPClass&)=delete;
    bool begin(const EspISPConfig&);
    void end();
    bool enterBootloader();
    bool connect(isp_target_chip_t target=ISP_TARGET_AUTO,uint32_t higher_baudrate=0);
    bool isConnected() const;
    const char *getTargetName() const;
    isp_target_chip_t getTarget() const;
    uint32_t getFlashSize();
    uint32_t getBaudRate() const;
    bool getMacAddress(uint8_t mac[6]);
    bool flashBuffer(uint32_t address,const uint8_t *buffer,size_t size);
    bool flashPROGMEM(uint32_t address,const uint8_t *buffer,size_t size);
    bool flashStream(uint32_t address,Stream &stream,size_t size);
    bool verifyMD5(uint32_t address,size_t size,const uint8_t expected_md5[16]);
    bool verifyMD5Hex(uint32_t address,size_t size,const char expected_hex[33]);
    void resetTarget();
    void onProgress(ISPProgressCallback);
    const char *getLastError() const;
    isp_error_t lastErrorCode() const;
    static int8_t slaveBootGPIO(isp_target_chip_t);
    static uint32_t slaveBootloaderOffset(isp_target_chip_t);
private:
    isp_internal_ctx_t *_ctx;
    isp_error_t _local_error;
    bool context();
};
extern EspISPClass EspISP;
