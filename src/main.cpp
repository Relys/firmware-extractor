#include <Arduino.h>

void mark_ota_reboot() {
  HAL_FLASH_Unlock();
  __HAL_FLASH_CLEAR_FLAG(0x35);
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x0800f800, 0x80a0);
}

void setup() {
  mark_ota_reboot();
}

void loop() {
}