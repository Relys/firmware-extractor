#define STACK_TOP 0x20005000

void startup();
void main();

// Define the vector table
unsigned int * myvectors[2] 
__attribute__ ((section("vectors")))= {
    (unsigned int *)    STACK_TOP,  // stack pointer
    (unsigned int *)    startup //+ 0x080000000     // code entry point
};

void startup()

    /* Now we are ready to start the main function */
    main();

    while (1) { }
}

