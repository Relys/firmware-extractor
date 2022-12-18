#include <Arduino.h>

#define FLASH_SERIAL_NUMBER_PART    0x0800FC0A
#define FLASH_SERIAL_NUMBER_SCALAR  0x0800FC30

#define BLE_FRAME_SIZE 20

#define BOOTLOADER_START            0x08000000
#define BOOTLOADER_END              0x08003000
#define SETTINGS_START              0x0800FC00
#define SETTINGS_END                0x0800FD00

#if ONEWHEEL_TYPE == XR
HardwareSerial OWSerial(USART1);
#else
HardwareSerial OWSerial(USART3);
#endif

/*
  Tells the Onewheel to reboot back into OTA mode on the next boot
*/
void mark_ota_reboot() {
  HAL_FLASH_Unlock();
  __HAL_FLASH_CLEAR_FLAG(0x35);
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x0800f800, 0x80a0);
}

uint32_t flash_read(uint32_t address) {
  return *(uint32_t*)address;
}

// void ble_send_serial_number() {
//   uint32_t serial_number = flash_read(FLASH_SERIAL_NUMBER_PART);
//   uint32_t serial_number_part_1 = flash_read(FLASH_SERIAL_NUMBER_SCALAR);
//   uint8_t temp, temp1, temp2;
//   uint8_t sn_digit_0, sn_digit_1, sn_digit_2, sn_digit_3, sn_digit_4, sn_digit_5;

//   if (serial_number != 0xffff || serial_number_part_1 != 0xffff) {
//     if (serial_number_part_1 != 0xfff) {
//       serial_number = serial_number + serial_number_part_1 * 0x10000;
//       OWSerial.print("One");
//       OWSerial.print(0);
//       OWSerial.print(0x20);
//       temp = serial_number >> 0x18;
//       OWSerial.print(temp);
//       temp1 = serial_number >> 0x10;
//       OWSerial.print(temp1);
//       OWSerial.print(temp ^ temp1 ^ 100);
//     }
//     OWSerial.print("One");
//     OWSerial.print(1);
//     temp2 = serial_number >> 8;
//     OWSerial.print(temp2);
//     OWSerial.print((uint8_t)serial_number);
//     OWSerial.print((uint8_t)serial_number ^ temp2 ^ 0x45);
//     OWSerial.print("One");
//     OWSerial.print(1);
//     OWSerial.print(0);
//     OWSerial.print("ow");
//     sn_digit_0 = (uint8_t)(((serial_number / 100000)) % 10) + 0x30;
//     sn_digit_1 = (uint8_t)(((serial_number / 10000)) % 10) + 0x30;
//     sn_digit_2 = (uint8_t)(((serial_number / 1000)) % 10) + 0x30;
//     sn_digit_3 = (uint8_t)(((serial_number / 100)) % 10) + 0x30;
//     sn_digit_4 = (uint8_t)(((serial_number / 10)) % 10) + 0x30;
//     sn_digit_5 = (uint8_t)(((serial_number % 10))) + 0x30;
//     OWSerial.print(sn_digit_0);
//     OWSerial.print(sn_digit_1);
//     OWSerial.print(sn_digit_2);
//     OWSerial.print(sn_digit_3);
//     OWSerial.print(sn_digit_4);
//     OWSerial.print(sn_digit_5);
//     OWSerial.print(sn_digit_5 ^ sn_digit_0 ^ 0x5d ^ sn_digit_1 ^ sn_digit_2 ^ sn_digit_3 ^ sn_digit_4);
//   }
// }

void setup_bluetooth() {
  OWSerial.begin(115200);

  for (int i = 0; i < 2400000; i++) {__asm("nop");}
  
  for (uint8_t i = 0; i < 8; i++)
    OWSerial.print(0);

  OWSerial.print("One ");
  OWSerial.print(0x11);
  OWSerial.print(0xFF);
  OWSerial.print(0xFF);
  OWSerial.print('U');

  // ble_send_serial_number();
}

void dump(uint32_t from, uint32_t to) {
  uint8_t buffer[20];
  uint32_t packetNumber = 0;
  uint8_t flip = 0;
  float progress = 0;
  for (uint32_t i = from; i < to; i += 20) {  
    progress = (float)(i - from) / (float)(to - from);
    // status_light_set_color(flip ? 0x000000 : progressColor, 1);
    memcpy(&buffer, (uint8_t*)i, sizeof(uint8_t) * BLE_FRAME_SIZE);
    for (int i = 0; i < BLE_FRAME_SIZE; i++)
      OWSerial.print(buffer[i]);
    delay(50);
  }
}

void setup() {  
  mark_ota_reboot();
  setup_bluetooth();
}

uint8_t hue = 255;

void loop() {
  if (OWSerial.available()) {
    char command = OWSerial.read();
    switch (command) {
      case 'b':
        dump(BOOTLOADER_START, BOOTLOADER_END);
      break;
      case 's':
        dump(SETTINGS_START, SETTINGS_END);
      break;
      case 'r':
        NVIC_SystemReset();
      break;
      // default:
        // show error command light
    }
  }
}