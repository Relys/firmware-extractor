SECTIONS
{
    /*.  = 0x00000000;         /* From 0x00000000 */
    .  = 0x08000000;         /* From 0x00000000 */

    .text : 
    {
        *(vectors)    /* Vector table */
        *(.text)      /* Program code */
    }
    .rodata : 
    {
        *(.rodata)    /* Read only data */
    }
    _DATA_ROM_START = .;

    .  = 0x20000000;  /* From 0x20000000 */      

    _DATA_RAM_START = .;
    _DATA_RAM_END = .;

    __bss_start__ = .;   /* Indicates where BSS section starts in RAM */
    .bss :
    {
        *(.bss)       /* Zero-filled run time allocate data memory */
    }
    __bss_end__ = .;     /* Indicates where BSS section ends in RAM */
}

