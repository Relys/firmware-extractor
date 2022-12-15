#define KEY_LOCATION 0x08000d54
#define BYTES_TO_READ 0x4

// Our custom magic headers to show we're sending the firmware data
#define HEADER_KEY_START 0x00
#define HEADER_KEY_END 0x00

// Important addresses
#define RAM_START ((volatile uint8_t * const) 0x20000000)

// Functions to call
#define flash_unlock        ((uint8_t (*)(void))0x08001092 + 1)
#define invalidate_lightbar ((void (*)(void))0x08000ab8 + 1)
#define send_bt_byte        ((void (*)(unsigned int))0x08001194 + 1)
#define read_bt_byte        ((unsigned int (*)(void))0x00000000)

#define ota_dma_timer_setup ((void (*)(void))0x08000b4a + 1)
#define ota_rcc_flash_setup ((uint32_t (*)(void))0x08000d04 + 1)
#define ota_uart_setup      ((void (*)(int, struct uart_config*))0x08000d74 + 1)
#define ota_rcc_clock_enable_something ((void (*)(unsigned int, int))0x08000f2a + 1)

#include <string.h>
#include <stdint.h>

unsigned int keys[4];
unsigned int last_cmd = 0x00;

#if defined (__GNUC__) /*!< GNU Compiler */
void __attribute__((naked))
delay_internal(unsigned long ulCount)
{
    __asm("    subs    r0, #1\n"
       "    bne     delay_internal\n"
       "    bx      lr");
}
#endif

void delay(unsigned long ms) {
    delay_internal(ms * (36000000/3000));
}

struct uart_config {
    uint32_t baud_rate;
    uint32_t param_1;
    uint16_t param_2;
    uint16_t param_3;
    uint16_t param_4;
};

void gpio_reset_pins(unsigned int* address, unsigned int value) {
    address[4] = value;
}

void gpio_reset_brr(unsigned int* address, unsigned int value) {
    address[5] = value;
}

void set_lightbar_index(uint8_t index, uint32_t hex) {
    uint8_t red   = (hex & 0xff0000) >> 16;
    uint8_t green = (hex & 0x00ff00) >> 8;
    uint8_t blue  = (hex & 0x0000ff);

    RAM_START[index * 3] = green;
    RAM_START[index * 3 + 1] = red;
    RAM_START[index * 3 + 2] = blue;
}

void set_lightbar(uint32_t hex) {
    for (uint8_t i = 0; i < 7; i++)
        set_lightbar_index(i, hex);

    invalidate_lightbar();
}

void main()
{
    ota_rcc_flash_setup();

    struct uart_config config;
    config.baud_rate = 115200;
    config.param_1 = 0;
    config.param_2 = 0;
    config.param_3 = 0;
    config.param_4 = 12;

    ota_uart_setup(0, &config);

    ota_dma_timer_setup();
    flash_unlock();
    ota_rcc_clock_enable_something(0x20, 1);

    set_lightbar(0xffffff);
    delay(5000);


    // set_lightbar(100, 255, 1);
    
    
    

    // ota_dma_timer_setup();

    // gpio_reset_pins((unsigned int*)0x4001100, 0x200);
    // set_lightbar(0xff, 0xbe, 0x00); // Show that we've successfully run the firmware
    // ota_rcc_clock_enable_something(0x20, 1);

    // // Ignore waiting for start header for now
    // //while (last_cmd != HEADER_KEY_START) {
    // //    last_cmd = read_bt_byte();
    // //}

    // set_lightbar(0xbe, 0x00, 0xff); // Acknowledge that we've received the start header

    // unlock_memory();

    // set_lightbar(0xff, 0x00, 0xbe); // Show that we've unlocked the flash and are going to read

    set_lightbar(0x00FF00);

    memcpy(&keys, (uint32_t*)(KEY_LOCATION), sizeof(uint32_t) * 4);
    delay(1000);

    set_lightbar(0x0000FF);

    for (int i = 0; i < BYTES_TO_READ; i++) {
        send_bt_byte(keys[i]);
    }

    delay(1000);

    send_bt_byte(HEADER_KEY_END);
    set_lightbar(0xFF0000);

    // set_lightbar(0x00, 0xff, 0x00); // "Ladies and gentlemen, we got him." (do a lightbar animation here)
}

