/*
 * Original info for this file is credited to Jacob Mossberg from his blog post:
 * https://jacobmossberg.se/posts/2018/08/11/run-c-program-bare-metal-on-arm-cortex-m3.html
 */

#define STACK_TOP 0x20005000

extern unsigned int _BSS_START;
extern unsigned int _BSS_END;
extern unsigned int _DATA_ROM_START;
extern unsigned int _DATA_RAM_START;
extern unsigned int _DATA_RAM_END;

void startup();
void main();

// Various memory addresses make unused called in OW firmware that we need to emulate
void empty_return();

// Define the vector table
unsigned int * myvectors[]
__attribute__ ((section("vectors")))= {
    (unsigned int *)    STACK_TOP,  // stack pointer
    (unsigned int *)    startup, // code entry point
};

void startup()
{
    /* Set memory in bss segment to zeros */
    unsigned int * bss_start_p = &_BSS_START; 
    unsigned int * bss_end_p = &_BSS_END;

    while(bss_start_p != bss_end_p)
    {
        *bss_start_p = 0;
        bss_start_p++;
    }

    /* Copy memory in data segment from its position
     * in the flash image to its position in the
     * read/write memory */
    unsigned int * data_rom_start_p = &_DATA_ROM_START;
    unsigned int * data_ram_start_p = &_DATA_RAM_START;
    unsigned int * data_ram_end_p = &_DATA_RAM_END;

    while(data_ram_start_p != data_ram_end_p)
    {
        *data_ram_start_p = *data_rom_start_p;
        data_ram_start_p++;
        data_rom_start_p++;
    }

    main();

    while (1) { }
}

void empty_return() {
    return; // Purposefully do nothing to emulate the stock `bl lr` instruction
}
