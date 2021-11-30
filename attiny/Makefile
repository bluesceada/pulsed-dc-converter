# Name: Makefile
# Author: Dennis Gnad
# Creation Date: 2010-06-12

DEVICE = attiny45
# we program the 8 MHz Int.Rc, and use a /2 software prescaler in code later:
F_CPU = 4000000	# in Hz
# see http://www.engbedded.com/fusecalc for fuse values
# Internal 8MHz, 4ms startup wait:
FUSE_L = 0xD2
# brown-out detection at 4.3V
FUSE_H = 0xDC
#PROGRAMMER = usbasp-clone -P usb
PROGRAMMER = avrisp -b 19200 -P /dev/ttyUSB0
# try to add -b 20 -i 5 for slow programmer devices

# Arduino ISP (as avrisp) pins:
# MISO °. . 5V (!) Avoid this pin on Due, Zero...
# SCK   . . MOSI
#       . . GND
#     +Pin 10: Reset

AVRDUDE = avrdude -p $(DEVICE) -c $(PROGRAMMER)

CFLAGS  = -Iusbdrv -I. -DDEBUG_LEVEL=0
OBJECTS = main.o

COMPILE = avr-gcc -Wall -Os -DF_CPU=$(F_CPU) $(CFLAGS) -mmcu=$(DEVICE)

all: flash

# symbolic targets:
help:
	@echo "make hex ....... to build main.hex"
	@echo "make program ... to flash fuses and firmware"
	@echo "make fuse ...... to flash the fuses"
	@echo "make flash ..... to flash the firmware (use this on metaboard)"
	@echo "make clean ..... to delete objects and hex file"

hex: main.hex

program: flash fuse

# rule for programming fuse bits:
fuse:
	@[ "$(FUSE_H)" != "" -a "$(FUSE_L)" != "" ] || \
		{ echo "*** Edit Makefile and choose values for FUSE_L and FUSE_H!"; exit 1; }
	$(AVRDUDE) -U hfuse:w:$(FUSE_H):m -U lfuse:w:$(FUSE_L):m

# rule for uploading firmware:
flash: main.hex
	$(AVRDUDE) -U flash:w:main.hex:i

# rule for deleting dependent files (those which can be built by Make):
clean:
	rm -f main.hex main.lst main.obj main.cof main.list main.map main.eep.hex main.elf *.o

# Generic rule for compiling C files:
.c.o:
	$(COMPILE) -c $< -o $@

# Generic rule for assembling Assembler source files:
.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

# Generic rule for compiling C to assembler, used for debugging only.
.c.s:
	$(COMPILE) -S $< -o $@

# file targets:

main.elf: $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS)

main.hex: main.elf
	rm -f main.hex main.eep.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	avr-size main.hex

# debugging targets:

disasm: main.elf
	avr-objdump -d main.elf

cpp:
	$(COMPILE) -E main.c
