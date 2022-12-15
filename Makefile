CC=arm-none-eabi-gcc
LD=arm-none-eabi-ld
CFLAGS=-O0 -g -mcpu=cortex-m3 -mthumb --specs=nosys.specs -T stm32.ld
LDFLAGS=-Tstm32.ld

FIRMWARE=firmware.c
STARTUP=startup.c
OUT=firmware.bin

#SRCS += STM32CubeF1/Drivers/CMSIS/Device/ST/STM32F1xx/Source/*.c
SRCS=firmware.c
SRCS += system_stm32f1xx.c
SRCS += STM32CubeF1/Drivers/STM32F1xx_HAL_Driver/Src/*.c

CFLAGS += -ISTM32CubeF1/Drivers/CMSIS/Device/ST/STM32F1xx/Include
CFLAGS += -ISTM32CubeF1/Drivers/CMSIS/Include
CFLAGS += -ISTM32CubeF1/Drivers/STM32F1xx_HAL_Driver/Inc

$(OUT): firmware.elf
	arm-none-eabi-objcopy -O binary firmware.elf $(OUT)
firmware.elf: $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf *.o *.elf $(OUT)

# arm-none-eabi-gcc -O0 -c -g -mcpu=cortex-m3 -mthumb -o firmware.o firmware.c
# arm-none-eabi-gcc -O0 -c -g -mcpu=cortex-m3 -mthumb -o startup.o startup.c
# arm-none-eabi-ld -Tstm32.ld -o firmware.elf startup.o firmware.o
# arm-none-eabi-objcopy -O binary firmware.elf firmware.bin
