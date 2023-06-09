#pragma once

#define XR 1
#define PINT 2
#define GT 3

#include <stdint.h>
#include <stdbool.h>

#if ONEWHEEL_TYPE == PINT
#include "ws2812_led.h"
#endif

#define GT_USART3_SR 0x40004800
#define GT_USART3_DR 0x40004804

#define BLE_FRAME_SIZE 20

#define LEGACY_OTA_FLAG_ADDRESS            0x0800F800
#define LEGACY_BOOTLOADER_START            0x08000000
#define LEGACY_BOOTLOADER_END              0x08003000
#define LEGACY_SETTINGS_START              0x0800FC00
#define LEGACY_SETTINGS_END                0x0800FD00
#define LEGACY_FLASH_SERIAL_NUMBER_PART    0x0800FC0A
#define LEGACY_FLASH_SERIAL_NUMBER_SCALAR  0x0800FC30

#define GT_BOOTLOADER_SETTINGS_START       0x08000000
#define GT_BOOTLOADER_SETTINGS_END         0x0800C000
#define GT_SERIAL_NUMBER_START             0x08010000
#define GT_SERIAL_NUMBER_END               0x0801000F
#define GT_FLASH_SERIAL_NUMBER_PART        0x08000000
#define GT_FLASH_SERIAL_NUMBER_SCALAR      0x08000000

enum StorageKeys {
  SerialNumber1 = 0xa504,
  SerialNumber2 = 0xa505,
  BootMode = 0xa509,
  HardwareRevision = 0xa506
};

uint32_t flash_read(uint32_t address);
uint16_t storage_search(uint32_t search_value, uint16_t **found_pointer);
uint16_t find_storage_end(uint16_t **found_pointer);

bool ble_available();
char ble_read_byte();
void ble_send(uint32_t data);
void mark_ota_reboot();
void setup_bluetooth();
void ble_send_serial_number();
void dump(uint32_t from, uint32_t to);
