#include <Arduino.h>
#include <stdint.h>
#include <string.h>

#define BLE_FRAME_SIZE 20

#define OTA_FLAG_ADDRESS            0x0800F800
#define BOOTLOADER_START            0x08000000
#define BOOTLOADER_END              0x08003000
#define SETTINGS_START              0x0800FC00
#define SETTINGS_END                0x0800FD00

#define FLASH_SERIAL_NUMBER_PART    0x0800FC0A
#define FLASH_SERIAL_NUMBER_SCALAR  0x0800FC30

#define XR 1
#define PINT 2

#if ONEWHEEL_TYPE == XR
HardwareSerial OWSerial(USART1);
#else
HardwareSerial OWSerial(USART3);
#endif

#if ONEWHEEL_TYPE == PINT
extern "C" {
#include "ws2812_led.h"
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

// Tells the Onewheel to reboot into OTA mode on the next boot
void mark_ota_reboot() {
  HAL_FLASH_Unlock();
  __HAL_FLASH_CLEAR_FLAG(0x35);
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, OTA_FLAG_ADDRESS, 0x80a0);
}

uint32_t flash_read(uint32_t address) {
  return *(uint32_t*)address;
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
      OWSerial.write(buffer[j]);
    delay(100);
  }

  // fill up the frame on the last packet
  if (frameSize < BLE_FRAME_SIZE && left < BLE_FRAME_SIZE)
    for (int i = 0; i < BLE_FRAME_SIZE - left; i++)
      OWSerial.write(0xFF);

  OWSerial.flush();
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
  if (OWSerial.available()) {
    char command = OWSerial.read();
    switch (command) {
      case 'b':
        dump(BOOTLOADER_START, BOOTLOADER_END);
      break;
      case 's':
        dump(SETTINGS_START, SETTINGS_END);
      break;
      case 'k':
        dump(0x08000d54, 0x08000d64);
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
