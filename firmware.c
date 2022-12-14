#define KEY_LOCATION 0x08000d54
#define BYTES_TO_READ 0x4

// Our custom magic headers to show we're sending the firmware data
#define HEADER_KEY_START 0x00
#define HEADER_KEY_END 0x00

#define unlock_memory   ((void (*)(void))0x08001092)
#define set_lightbar    ((void (*)(unsigned int, unsigned int, unsigned int))0x08000c46)
#define read_bt_byte    ((unsigned int (*)(void))0x00000000)
#define send_bt_byte    ((void (*)(unsigned int))0x08001194)

#include <string.h>

unsigned int keys[4];
unsigned int last_cmd = 0x00;

void main()
{
    set_lightbar(0xff, 0xbe, 0x00); // Show that we've successfully run the firmware

    while (last_cmd != HEADER_KEY_START) {
        last_cmd = read_bt_byte();
    }

    set_lightbar(0xbe, 0x00, 0xff); // Acknowledge that we've received the start header

    unlock_memory();

    set_lightbar(0xff, 0x00, 0xbe); // Show that we've unlocked the flash and are going to read

    for (int i = 0; i < BYTES_TO_READ; i++) {
        memcpy(&keys[i], (unsigned int*)(KEY_LOCATION + (i * sizeof(unsigned int))), sizeof(unsigned int));
    }

    for (int i = 0; i < BYTES_TO_READ; i++) {
        send_bt_byte(keys[i]);
    }

    send_bt_byte(HEADER_KEY_END);

    set_lightbar(0x00, 0xff, 0x00); // "Ladies and gentlemen, we got him." (do a lightbar animation here)
}

