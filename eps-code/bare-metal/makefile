
USERNAME=craig

TC  =C:/users/$(USERNAME)/AppData/Local/Arduino15/packages/arduino/tools/arm-none-eabi-gcc/4.8.3-2014q1/bin
PROG=C:/users/$(USERNAME)/AppData/Local/Arduino15/packages/arduino/tools/bossac/bossac-1.7.0

CC=$(TC)/arm-none-eabi-gcc

CFLAGS=-mcpu=cortex-m3 -mthumb --specs=nosys.specs -nostdlib -g -Os

all : bare-metal.bin

main.o: main.c
	@echo Compiling $<
	@$(CC) -c main.c -o main.o $(CFLAGS)

startup.o: startup.c
	@echo Compiling $<
	@$(CC) -c startup.c -o startup.o $(CFLAGS)

bare-metal.elf : main.o startup.o flash.ld
	@echo Linking $@
	@$(CC) startup.o main.o -o bare-metal.elf -mcpu=cortex-m3 -mthumb --specs=nosys.specs -nostdlib "-Tflash.ld" "-Wl,-Map,bare-metal.map" -nostartfiles

bare-metal.bin : bare-metal.elf
	@echo Creating bin file $@
	@$(TC)/arm-none-eabi-objcopy  -O binary  bare-metal.elf bare-metal.bin

clean:
	rm -f main.o startup.o bare-metal.bin bare-metal.elf

prog:

	echo Programming code...
	$(PROG)/bossac --port=COM68 -U true -i -u -e -w -b "bare-metal.bin" -v -R
