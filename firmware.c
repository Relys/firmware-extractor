#define KEY_LOCATION 0x08000d54
#define BYTES_TO_READ 16

// Our custom magic headers to show we're sending the firmware data
#define HEADER_KEY_START 0x00
#define HEADER_KEY_END 0x00

// Important addresses
#define RAM_START ((volatile uint8_t * const) 0x20000000)
#define GPIOD 0x40011400
#define PTR_FLASH_SERIAL_NUMBER_0   ((volatile uint8_t * const) 0x20000054)
#define PTR_FLASH_SERIAL_NUMBER_1   ((volatile uint8_t * const) 0x20000058)

// Functions to call
#define flash_unlock        ((uint8_t (*)(void))0x08001092 + 1)
#define status_light_invalidate ((void (*)(void))0x08000ab8 + 1)
#define ble_send_byte        ((void (*)(uint16_t))0x08001194 + 1)
#define ble_receive_byte        ((unsigned int (*)(void))0x00000000)

#define dma_timer_init      ((void (*)(void))0x08000b4a + 1)
#define hal_init            ((uint32_t (*)(void))0x08000d04 + 1)
#define uart_init           ((void (*)(int, struct uart_config*))0x08000d74 + 1)
#define rcc_clock_enable_configure ((void (*)(unsigned int, int))0x08000f2a + 1)

#define gpio_init_typedef   ((void (*)(struct gpio_config*))0x08001056 + 1)
#define hal_gpio_init       ((void (*)(uint32_t *, struct gpio_config*))0x08000f9c + 1)
#define gpio_set_reset_pins ((void (*)(uint32_t *, uint32_t))0x08001066 + 1)

#include <string.h>
#include <stdint.h>

uint8_t keys[BYTES_TO_READ];
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

struct gpio_config {
    uint16_t pin;
    uint8_t speed;
    uint8_t mode;
};

void gpio_reset_pins(unsigned int* address, unsigned int value) {
    address[4] = value;
}

void gpio_reset_brr(unsigned int* address, unsigned int value) {
    address[5] = value;
}

void status_light_set_pixel(uint8_t index, uint32_t hex) {
    uint8_t red   = (hex & 0xff0000) >> 16;
    uint8_t green = (hex & 0x00ff00) >> 8;
    uint8_t blue  = (hex & 0x0000ff);

    RAM_START[index * 3] = green;
    RAM_START[index * 3 + 1] = red;
    RAM_START[index * 3 + 2] = blue;
}

void status_light_set_color(uint32_t hex) {
    for (uint8_t i = 0; i < 7; i++)
        status_light_set_pixel(i, hex);

    status_light_invalidate();
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
            ble_send_byte('O');
            ble_send_byte('n');
            ble_send_byte('e');
            ble_send_byte(0);
            ble_send_byte(0x20);
            temp = serial_number >> 0x18;
            ble_send_byte(temp);
            temp1 = serial_number >> 0x10;
            ble_send_byte(temp1);
            ble_send_byte(temp ^ temp1 ^ 100);
        }
        ble_send_byte('O');
        ble_send_byte('n');
        ble_send_byte('e');
        ble_send_byte(1);
        temp2 = serial_number >> 8;
        ble_send_byte(temp2);
        ble_send_byte((uint8_t)serial_number);
        ble_send_byte((uint8_t)serial_number ^ temp2 ^ 0x45);
        ble_send_byte('O');
        ble_send_byte('n');
        ble_send_byte('e');
        ble_send_byte(1);
        ble_send_byte(0);
        ble_send_byte('o');
        ble_send_byte('w');
        sn_digit_0 = (uint8_t)(((serial_number / 100000)) % 10) + 0x30;
        sn_digit_1 = (uint8_t)(((serial_number / 10000)) % 10) + 0x30;
        sn_digit_2 = (uint8_t)(((serial_number / 1000)) % 10) + 0x30;
        sn_digit_3 = (uint8_t)(((serial_number / 100)) % 10) + 0x30;
        sn_digit_4 = (uint8_t)(((serial_number / 10)) % 10) + 0x30;
        sn_digit_5 = (uint8_t)(((serial_number % 10))) + 0x30;
        ble_send_byte(sn_digit_0);
        ble_send_byte(sn_digit_1);
        ble_send_byte(sn_digit_2);
        ble_send_byte(sn_digit_3);
        ble_send_byte(sn_digit_4);
        ble_send_byte(sn_digit_5);
        ble_send_byte(sn_digit_5 ^ sn_digit_0 ^ 0x5d ^ sn_digit_1 ^ sn_digit_2 ^ sn_digit_3 ^ sn_digit_4);
    }
}

void ble_init_update_mode() {
    for (int i = 0; i < 2400000; i++) {__asm("nop");}

    for (uint8_t i = 0; i < 8; i++)
        ble_send_byte(0);
    ble_send_byte('O');
    ble_send_byte('n');
    ble_send_byte('e');
    ble_send_byte(' ');
    ble_send_byte(0x11);
    ble_send_byte(0xFF);
    ble_send_byte(0xFF);
    ble_send_byte('U');

    status_light_set_color(0x00FFFF);

    ble_send_serial_number();
}

void main()
{
    hal_init();

    struct uart_config bt_config;
    bt_config.baud_rate = 115200;
    bt_config.param_1 = 0;
    bt_config.param_2 = 0;
    bt_config.param_3 = 12;
    bt_config.param_4 = 0;

    uart_init(0, &bt_config);
    dma_timer_init();
    rcc_clock_enable_configure(0x20, 1);
    
    struct gpio_config pin_config;
    gpio_init_typedef(&pin_config);
    pin_config.pin = 512;
    pin_config.mode = 16;
    hal_gpio_init((uint32_t*)GPIOD, &pin_config);

    gpio_set_reset_pins((uint32_t*)GPIOD, 0x200);
    flash_unlock();

    ble_init_update_mode();
    status_light_set_color(0x0000FF);

    delay(200);

    // // Ignore waiting for start header for now
    // //while (last_cmd != HEADER_KEY_START) {
    // //    last_cmd = read_bt_byte();
    // //}

    // set_lightbar(0xbe, 0x00, 0xff); // Acknowledge that we've received the start header

    // set_lightbar(0xff, 0x00, 0xbe); // Show that we've unlocked the flash and are going to read

    // set_lightbar(0x00FF00);

    memcpy(&keys, (uint8_t*)(KEY_LOCATION), sizeof(uint8_t) * BYTES_TO_READ);
    // delay(1000);

    // set_lightbar(0x0000FF);

    for (int i = 0; i < BYTES_TO_READ; i++) {
        ble_send_byte(keys[i]);
    }

    // delay(1000);

    // ble_send_byte(HEADER_KEY_END);
    status_light_set_color(0xFF0000);

    // set_lightbar(0x00, 0xff, 0x00); // "Ladies and gentlemen, we got him." (do a lightbar animation here)
}

