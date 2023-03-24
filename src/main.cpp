#include <Arduino.h>
#include <stdint.h>
#include <string.h>
#include "common.h"

#if ONEWHEEL_TYPE == XR
HardwareSerial OWSerial(USART1);
#else
HardwareSerial OWSerial(USART3);
#endif

#if ONEWHEEL_TYPE == PINT
#include "ws2812_led.h"
extern "C" {
void GPIO_PortB_Output_Init();
}

#define LEDS 7
#define BUFFER_PER_PIXEL 4
#define FRAMEBUFFER_SIZE LEDS * BUFFER_PER_PIXEL

struct pixel {
  uint8_t g;
  uint8_t r;
  uint8_t b;
  uint8_t w;
};

struct pixel framebuffer[FRAMEBUFFER_SIZE];
struct led_channel_info channels[1];
#endif

#if ONEWHEEL_TYPE == XR || ONEWHEEL_TYPE == PINT
#define OTA_FLAG_ADDRESS            LEGACY_OTA_FLAG_ADDRESS
#define STAGE_ONE_START             LEGACY_BOOTLOADER_START
#define STAGE_ONE_END               LEGACY_BOOTLOADER_END
#define STAGE_TWO_START             LEGACY_SETTINGS_START
#define STAGE_TWO_END               LEGACY_SETTINGS_END
#define FLASH_SERIAL_NUMBER_PART    LEGACY_FLASH_SERIAL_NUMBER_PART
#define FLASH_SERIAL_NUMBER_SCALAR  LEGACY_FLASH_SERIAL_NUMBER_SCALAR
#elif ONEWHEEL_TYPE == GT
#define STAGE_ONE_START             GT_BOOTLOADER_SETTINGS_START
#define STAGE_ONE_END               GT_BOOTLOADER_SETTINGS_END
#define STAGE_TWO_START             GT_SERIAL_NUMBER_START
#define STAGE_TWO_END               GT_SERIAL_NUMBER_END
#endif

// Tells the Onewheel to reboot into OTA mode on the next boot
#if ONEWHEEL_TYPE != GT
#define FRAME_DELAY 250

void mark_ota_reboot() {
  HAL_FLASH_Unlock();
  __HAL_FLASH_CLEAR_FLAG(0x35);
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, OTA_FLAG_ADDRESS, 0x80a0);
}
#else
// GT version
#define FRAME_DELAY 75 

void mark_ota_reboot() {
  HAL_FLASH_Unlock();
  __HAL_FLASH_CLEAR_FLAG(0x35);
  
  uint16_t *last_storage_address;
  find_storage_end(&last_storage_address);

  // Append to the end of the storage and write that we're in OTA mode
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, (uint32_t)last_storage_address, 0x80a0);
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, (uint32_t)last_storage_address + 2, StorageKeys::BootMode);
}
#endif

uint32_t flash_read(uint32_t address) {
  return *(uint32_t*)address;
}

#if ONEWHEEL_TYPE == XR || ONEWHEEL_TYPE == PINT
void setup_bluetooth() {
  OWSerial.begin(115200);

  for (int i = 0; i < 2400000; i++)
    __asm("nop");
  
  for (uint8_t i = 0; i < 8; i++)
    OWSerial.print(0, HEX);

  OWSerial.print("One ");
  OWSerial.print(0x11, HEX);
  OWSerial.print(0xFF, HEX);
  OWSerial.print(0xFF, HEX);
  OWSerial.print('U');

  ble_send_serial_number();
}

void ble_send_serial_number() {
  uint32_t serial_number = flash_read(FLASH_SERIAL_NUMBER_PART);
  uint32_t serial_number_part_1 = flash_read(FLASH_SERIAL_NUMBER_SCALAR);
  uint8_t temp, temp1, temp2;
  uint8_t sn_digit_0, sn_digit_1, sn_digit_2, sn_digit_3, sn_digit_4, sn_digit_5;

  if (serial_number != 0xffff || serial_number_part_1 != 0xffff) {
    if (serial_number_part_1 != 0xfff) {
      serial_number = serial_number + serial_number_part_1 * 0x10000;
      OWSerial.print("One");
      OWSerial.print(0, HEX);
      OWSerial.print(0x20, HEX);
      temp = serial_number >> 0x18;
      OWSerial.print(temp, HEX);
      temp1 = serial_number >> 0x10;
      OWSerial.print(temp1, HEX);
      OWSerial.print(temp ^ temp1 ^ 100, HEX);
    }
    OWSerial.print("One");
    OWSerial.print(1, HEX);
    temp2 = serial_number >> 8;
    OWSerial.print(temp2, HEX);
    OWSerial.print((uint8_t)serial_number, HEX);
    OWSerial.print((uint8_t)serial_number ^ temp2 ^ 0x45, HEX);
    OWSerial.print("One");
    OWSerial.print(1, HEX);
    OWSerial.print(0, HEX);
    OWSerial.print("ow");
    sn_digit_0 = (uint8_t)(((serial_number / 100000)) % 10) + 0x30;
    sn_digit_1 = (uint8_t)(((serial_number / 10000)) % 10) + 0x30;
    sn_digit_2 = (uint8_t)(((serial_number / 1000)) % 10) + 0x30;
    sn_digit_3 = (uint8_t)(((serial_number / 100)) % 10) + 0x30;
    sn_digit_4 = (uint8_t)(((serial_number / 10)) % 10) + 0x30;
    sn_digit_5 = (uint8_t)(((serial_number % 10))) + 0x30;
    OWSerial.print(sn_digit_0, HEX);
    OWSerial.print(sn_digit_1, HEX);
    OWSerial.print(sn_digit_2, HEX);
    OWSerial.print(sn_digit_3, HEX);
    OWSerial.print(sn_digit_4, HEX);
    OWSerial.print(sn_digit_5, HEX);
    OWSerial.print(sn_digit_5 ^ sn_digit_0 ^ 0x5d ^ sn_digit_1 ^ sn_digit_2 ^ sn_digit_3 ^ sn_digit_4, HEX);
  }
}

#elif ONEWHEEL_TYPE == GT
void setup_bluetooth() {
  delay(50);
  ble_send_serial_number();
}

void ble_send_serial_number() {
  uint16_t* hardware_revision_ptr;
  storage_search(StorageKeys::HardwareRevision, &hardware_revision_ptr);

  uint16_t* serial_number_ptr;
  storage_search(StorageKeys::SerialNumber1, &serial_number_ptr);

  uint16_t* serial_number2_ptr;
  storage_search(StorageKeys::SerialNumber2, &serial_number2_ptr);

  uint16_t hardware_revision = *hardware_revision_ptr;
  uint16_t serial_number = *serial_number_ptr;
  uint16_t serial_number_2 = *serial_number2_ptr;

  uint32_t temp, temp2, temp3;
  uint8_t temp4;
  uint8_t digit_0, digit_1, digit_2, digit_3, digit_4, digit_5;
  // ota_set_status_light(100,0xff,1);
  HAL_FLASH_Unlock();
  ble_send(0);
  ble_send(0);
  ble_send(0);
  ble_send(0);
  ble_send(0);
  ble_send(0);
  ble_send(0);
  ble_send(0);
  ble_send('O');
  ble_send('n');
  ble_send('e');
  ble_send(0);
  ble_send(1);
  temp = serial_number_2;
  ble_send((uint32_t)(temp >> 8));
  temp2 = serial_number_2;
  ble_send((uint32_t)(byte)temp2);
  ble_send(((uint32_t)temp2 ^ temp >> 8 ^ 0x45) & 0xff);
  ble_send('O');
  ble_send('n');
  ble_send('e');
  ble_send(0);
  ble_send(' ');
  temp = serial_number;
  ble_send((uint32_t)(temp >> 8));
  temp2 = serial_number;
  ble_send((uint32_t)(byte)temp2);
  ble_send(((uint32_t)temp2 ^ temp >> 8 ^ 100) & 0xff);
  temp = serial_number_2;
  temp2 = serial_number;
  if ((uint32_t)temp2 + (uint32_t)temp != 0) {
    ble_send('O');
    ble_send('n');
    ble_send('e');
    ble_send(1);
    ble_send(0);
    ble_send('o');
    ble_send('w');
    temp2 = serial_number;
    temp = serial_number_2;
    digit_0 = (((uint32_t)temp2 * 0x10000 + (uint32_t)temp) / 100000) % 10 + 0x30;
    ble_send(digit_0);
    temp2 = serial_number;
    temp = serial_number_2;
    digit_1 = (((uint32_t)temp2 * 0x10000 + (uint32_t)temp) / 10000) % 10 + 0x30;
    ble_send(digit_1);
    temp2 = serial_number;
    temp = serial_number_2;
    digit_2 = (((uint32_t)temp2 * 0x10000 + (uint32_t)temp) / 1000) % 10 + 0x30;
    ble_send(digit_2);
    temp2 = serial_number;
    temp = serial_number_2;
    digit_3 = (((uint32_t)temp2 * 0x10000 + (uint32_t)temp) / 100) % 10 + 0x30;
    ble_send(digit_3);
    temp2 = serial_number;
    temp = serial_number_2;
    digit_4 = (((uint32_t)temp2 * 0x10000 + (uint32_t)temp) / 10) % 10 + 0x30;
    ble_send(digit_4);
    temp2 = serial_number;
    temp = serial_number_2;
    digit_5 = (uint32_t)temp2 * 0x10000 + (uint32_t)temp;
    digit_5 = digit_5 % 10 + 0x30;
    ble_send(digit_5);
    ble_send(digit_5 ^ digit_0 ^ 0x5d ^ digit_1 ^ digit_2 ^ digit_3 ^ digit_4);
  }
  ble_send('O');
  ble_send('n');
  ble_send('e');
  ble_send(0);
  ble_send(24);
  temp = hardware_revision;
  ble_send((uint32_t)temp * 1000 >> 8 & 0xff);
  temp3 = hardware_revision;
  temp4 = (char)temp3 * -0x18;
  ble_send((uint32_t)temp4);
  ble_send((uint32_t)(byte)(temp4 ^ (byte)((uint32_t)temp * 1000 >> 8) ^ 0x5c));
}
#endif

// Dumps the contents of the flash memory to the serial port
void dump(uint32_t from, uint32_t to) {
  size_t left = to - from;
  uint8_t buffer[BLE_FRAME_SIZE];  
  size_t frameSize;
  for (uint32_t i = from; i <= to; i += BLE_FRAME_SIZE) {
    left = to - i;
    frameSize = left < BLE_FRAME_SIZE ? left : BLE_FRAME_SIZE;

    //progress = (float)(i - from) / (float)(to - from);
    memcpy(&buffer, (uint8_t*)i, sizeof(uint8_t) * frameSize);
    for (size_t j = 0; j < frameSize; j++)
#if ONEWHEEL_TYPE == GT
      ble_send(buffer[j]);
#else
      OWSerial.write(buffer[j]);
#endif

    while (true) {
      if (ble_available() && ble_read_byte() == 'n') {
        break;
      }
    }
  }

  // fill up the frame on the last packet
  if (frameSize < BLE_FRAME_SIZE && left < BLE_FRAME_SIZE)
    for (int i = 0; i < BLE_FRAME_SIZE - left; i++)
#if ONEWHEEL_TYPE == GT
      ble_send(0xFF);
#else
      OWSerial.write(0xFF);
      
    OWSerial.flush();
#endif
}

#if ONEWHEEL_TYPE == PINT
void set_color(led_channel_info *channel, pixel color) { 
  auto framebuffer = channel->framebuffer;
  for (int i = 0; i < LEDS; i++) {
    memcpy((void*)&framebuffer[i * BUFFER_PER_PIXEL], &color, sizeof(pixel));
  }
}

pixel white { .g = 0, .r = 0, .b = 0, .w = 0xFF };
pixel red { .g = 0, .r = 0xFF, .b = 0, .w = 0 };
#endif

void setup() {
#if ONEWHEEL_TYPE == PINT
  GPIO_PortB_Output_Init();

  __enable_irq();
  channels[0].framebuffer = (const uint8_t*) framebuffer;
  channels[0].length = LEDS * sizeof(struct pixel);

  // doesn't work properly just yet. timing needs to be adjusted I think
  // but works enough
  set_color(&channels[0], red);
  ws2812_init();
#endif
  
  HAL_FLASH_Unlock();  
  __HAL_FLASH_CLEAR_FLAG(0x35);
  mark_ota_reboot();
  setup_bluetooth();
}

void loop() {
  if (ble_available()) {
    char command = ble_read_byte();
    switch (command) {
      case 'b':
        dump(STAGE_ONE_START, STAGE_ONE_END);
      break;
      case 's':
        dump(STAGE_TWO_START, STAGE_TWO_END);
      break;
      case 'r':
        NVIC_SystemReset();
      break;
      // default:
        // show error command light
    }
  }

#if ONEWHEEL_TYPE == PINT
  __disable_irq();
  ws2812_refresh(channels, GPIOB);
  __enable_irq();
#endif
}

uint16_t storage_search(uint32_t search_value, uint16_t **found_pointer) {
  uint16_t *search_address = (uint16_t*)0x0800BFFE;

  for (; search_address > (uint16_t*)0x08008003; search_address -= 2) {
    if (*search_address == search_value) {
      *found_pointer = search_address - 1;

      return 1;
    }
  }

  return 0;
}

uint16_t find_storage_end(uint16_t **found_pointer) {
  uint16_t *search_address = (uint16_t*)0x0800BFFE;

  for (; search_address > (uint16_t*)0x08008003; search_address -= 2) {
    if (*search_address != 0xFFFF) {
      *found_pointer = search_address + 1;

      return 1;
    }
  }

  return 0;
}

bool ble_available() {
#if ONEWHEEL_TYPE == GT
  uint32_t temp = *(uint32_t*)GT_USART3_SR;
  return (int)(temp << 26) < 0;
#else
  return OWSerial.available();
#endif
}

char ble_read_byte() {
#if ONEWHEEL_TYPE == GT
  return *(uint32_t*)GT_USART3_DR & 0xff;
#else
  return OWSerial.read();
#endif
}

void ble_send(uint32_t data) {
  uint32_t temp;
  do {
    temp = *(uint32_t*)GT_USART3_SR;
  } while (-1 < (int)(temp << 0x18));
  *(uint32_t*)GT_USART3_DR = (uint32_t)data;
  do {
    temp = *(uint32_t*)GT_USART3_SR;
  } while (-1 < (int)(temp << 0x18));
}
