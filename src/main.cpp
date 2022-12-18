#include <Arduino.h>

// todo: switch to using reading from flash instead of these RAM addresses
#define PTR_FLASH_SERIAL_NUMBER_0   0x20000054
#define PTR_FLASH_SERIAL_NUMBER_1   0x20000058

#define FLASH_SERIAL_NUMBER_PART    0x0800FC0A
#define FLASH_SERIAL_NUMBER_SCALAR  0x0800FC30

#define BLE_FRAME_SIZE 20

#define BOOTLOADER_START            0x08000000
#define BOOTLOADER_END              0x08003000
#define SETTINGS_START              0x0800FC00
#define SETTINGS_END                0x0800FD00

HardwareSerial Serial3(USART3);
/*
  Tells the Onewheel to reboot back into OTA mode on the next boot
*/
void mark_ota_reboot() {
  HAL_FLASH_Unlock();
  __HAL_FLASH_CLEAR_FLAG(0x35);
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x0800f800, 0x80a0);
}

void ble_send_serial_number() {
  uint16_t* serial_number_0_pointer = (uint16_t*)PTR_FLASH_SERIAL_NUMBER_0;
  uint16_t* serial_number_1_pointer = (uint16_t*)PTR_FLASH_SERIAL_NUMBER_1;
  uint32_t serial_number = (uint32_t)*serial_number_0_pointer;
  uint32_t serial_number_part_1 = (uint32_t)*serial_number_1_pointer;
  uint8_t temp, temp1, temp2;
  uint8_t sn_digit_0, sn_digit_1, sn_digit_2, sn_digit_3, sn_digit_4, sn_digit_5;

  if (serial_number != 0xffff || serial_number_part_1 != 0xffff) {
    if (serial_number_part_1 != 0xfff) {
      serial_number = serial_number + serial_number_part_1 * 0x10000;
      Serial3.print("One");
      Serial3.print(0);
      Serial3.print(0x20);
      temp = serial_number >> 0x18;
      Serial3.print(temp);
      temp1 = serial_number >> 0x10;
      Serial3.print(temp1);
      Serial3.print(temp ^ temp1 ^ 100);
    }
    Serial3.print("One");
    Serial3.print(1);
    temp2 = serial_number >> 8;
    Serial3.print(temp2);
    Serial3.print((uint8_t)serial_number);
    Serial3.print((uint8_t)serial_number ^ temp2 ^ 0x45);
    Serial3.print("One");
    Serial3.print(1);
    Serial3.print(0);
    Serial3.print("ow");
    sn_digit_0 = (uint8_t)(((serial_number / 100000)) % 10) + 0x30;
    sn_digit_1 = (uint8_t)(((serial_number / 10000)) % 10) + 0x30;
    sn_digit_2 = (uint8_t)(((serial_number / 1000)) % 10) + 0x30;
    sn_digit_3 = (uint8_t)(((serial_number / 100)) % 10) + 0x30;
    sn_digit_4 = (uint8_t)(((serial_number / 10)) % 10) + 0x30;
    sn_digit_5 = (uint8_t)(((serial_number % 10))) + 0x30;
    Serial3.print(sn_digit_0);
    Serial3.print(sn_digit_1);
    Serial3.print(sn_digit_2);
    Serial3.print(sn_digit_3);
    Serial3.print(sn_digit_4);
    Serial3.print(sn_digit_5);
    Serial3.print(sn_digit_5 ^ sn_digit_0 ^ 0x5d ^ sn_digit_1 ^ sn_digit_2 ^ sn_digit_3 ^ sn_digit_4);
  }
}

void setup_bluetooth() {
  Serial3.begin(115200);

  for (int i = 0; i < 2400000; i++) {__asm("nop");}
  
  for (uint8_t i = 0; i < 8; i++)
    Serial3.print(0);

  Serial3.print("One ");
  Serial3.print(0x11);
  Serial3.print(0xFF);
  Serial3.print(0xFF);
  Serial3.print('U');

  ble_send_serial_number();
}

void dump(uint32_t from, uint32_t to, uint32_t progressColor) {
  uint8_t buffer[20];
  uint32_t packetNumber = 0;
  uint8_t flip = 0;
  for (uint32_t i = from; i < to; i += 20) {  
    packetNumber++;
    if (packetNumber % 10 == 0) flip = flip == 1 ? 0 : 1;
      // status_light_set_color(flip ? 0x000000 : progressColor, 1);
      memcpy(&buffer, (uint8_t*)i, sizeof(uint8_t) * BLE_FRAME_SIZE);
      for (int i = 0; i < BLE_FRAME_SIZE; i++)
        Serial3.print(buffer[i]);
    delay(50);
  }
}

void setup() {
  mark_ota_reboot();
  setup_bluetooth();
}

void loop() {
  if (Serial3.available()) {
    char command = Serial3.read();
    switch (command) {
      case 'b':
        dump(BOOTLOADER_START, BOOTLOADER_END, 0x004000);
      break;
      case 's':
        dump(SETTINGS_START, SETTINGS_END, 0x004040);
      break;
      case 'r':
        NVIC_SystemReset();
      break;
      // default:
        // show error command light
    }
  }
}