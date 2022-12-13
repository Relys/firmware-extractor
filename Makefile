CC=arm-none-eabi-gcc
LD=arm-none-eabi-ld
CFLAGS=-O0 -g -c -mcpu=cortex-m3 -mthumb
LDFLAGS=-Tstm32.ld

FIRMWARE=firmware.c
STARTUP=startup.c
OUT=firmware.bin
SIGNED_OUT=signed-firmware.bin

$(OUT): firmware.elf
	arm-none-eabi-objcopy -O binary firmware.elf $(OUT)
firmware.elf: startup.o firmware.o
	$(LD) $(LDFLAGS) -o firmware.elf startup.o firmware.o
startup.o: $(STARTUP)
	$(CC) $(CFLAGS) -o startup.o $(STARTUP)
firmware.o: $(FIRMWARE)
	$(CC) $(CFLAGS) -o firmware.o $(FIRMWARE)

# $(KEY) passed in as environment variable to prevent accidental commits (-K only takes direct hex string)
sign:
	openssl enc -d -aes-128-ecb -nosalt -nopad -K $(KEY) -in $(OUT) -out $(SIGNED_OUT)

clean:
	rm -rf *.o *.elf $(OUT) $(SIGNED_OUT)

# arm-none-eabi-gcc -O0 -c -g -mcpu=cortex-m3 -mthumb -o firmware.o firmware.c
# arm-none-eabi-gcc -O0 -c -g -mcpu=cortex-m3 -mthumb -o startup.o startup.c
# arm-none-eabi-ld -Tstm32.ld -o firmware.elf startup.o firmware.o
# arm-none-eabi-objcopy -O binary firmware.elf firmware.bin
