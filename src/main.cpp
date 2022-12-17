#include <Arduino.h>
#include <FlashStorage_STM32.h>

void setup() {
    // put OTA flag back into in for reboot
    EEPROM.put(0x0800f800, 0x80a0);
    EEPROM.commit();
}

void loop() {

}