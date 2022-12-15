#define KEY_LOCATION 0x08000d54

#include <string.h>
#include "stm32f1xx.h"
#include "system_stm32f1xx.h"

unsigned int keys[4];

void main()
{
    for (int i = 0; i < 4; i++) {
        memcpy(&keys[i], (unsigned int*)(KEY_LOCATION + (i * sizeof(unsigned int))), sizeof(unsigned int));
    }

    unsigned int* test;

    for (int i = 0; i < 4; i++) {
        memcpy(test, &keys[i], sizeof(unsigned int));
    }

}

