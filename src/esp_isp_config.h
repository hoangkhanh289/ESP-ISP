#pragma once
// Defaults are evaluated by the public facade; core block size is fixed at 1024.
#ifndef ESP_ISP_INIT_BAUD
#define ESP_ISP_INIT_BAUD 115200u
#endif
#ifndef ESP_ISP_FLASH_BAUD
#define ESP_ISP_FLASH_BAUD 921600u
#endif
#ifndef ESP_ISP_RESET_HOLD_MS
#define ESP_ISP_RESET_HOLD_MS 100u
#endif
#ifndef ESP_ISP_BOOT_HOLD_MS
#define ESP_ISP_BOOT_HOLD_MS 50u
#endif
#ifndef ESP_ISP_STREAM_TIMEOUT_MS
#define ESP_ISP_STREAM_TIMEOUT_MS 5000u
#endif
#define ESP_ISP_CORE_BLOCK_BYTES 1024u
#define ESP_ISP_ABI_VERSION 1u
