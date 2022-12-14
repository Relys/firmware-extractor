#define FAKE_STACK_POINTER 0x20005000

void startup();
void main();

// Various memory addresses make unused called in OW firmware that we need to emulate
void empty_return();

// Define the modified psuedo vector table to match stock OW firmware. 
unsigned int * myvectors[] __attribute__ ((section("vectors"))) = {
    (unsigned int *)    FAKE_STACK_POINTER, // Not sure what this is (different from stack pointer definition)
    (unsigned int *)    startup,            // Our codes entrypoint 
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    0x00000000,         // Empty spacer
    (unsigned int *)    0x00000000,         // Empty spacer
    (unsigned int *)    0x00000000,         // Empty spacer
    (unsigned int *)    0x00000000,         // Empty spacer
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    0x00000000,         // Empty spacer

    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused

    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused

    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused

    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused

    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused

    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused

    (unsigned int *)    empty_return,       // Unused
    (unsigned int *)    empty_return,       // Unused
};

void startup()
{
    // Not sure if we need to do anything here anymore besides run main

    main();

    while (1) { }
}

void empty_return() {
    return; // Purposefully do nothing to emulate the stock `bx lr` instruction
}
