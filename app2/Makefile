ARMGNU = arm-none-eabi

DIR = $(shell basename `pwd`)
TARGET = $(DIR).bin
SDTARGET = ../things_to_copy_to_your_sd_card/$(TARGET)
DEPS = ../*.h

COPS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding

all : $(TARGET) 

%.o : %.s
	$(ARMGNU)-as $< -o $@

%.o : %.c $(DEPS)
	$(ARMGNU)-gcc $(COPS) -c $< -o $@

OBJECTS := $(patsubst %.s,%.o,$(wildcard *.s)) $(patsubst %.c,%.o,$(wildcard *.c))

$(TARGET) : $(OBJECTS)
	$(ARMGNU)-ld $(OBJECTS) -T memmap -o app.elf
	$(ARMGNU)-objdump -D app.elf > app.list
	$(ARMGNU)-objcopy app.elf -O binary $(TARGET)
	cp $(TARGET) $(SDTARGET)

clean :
	rm -f *.o
	rm -f *.bin
	rm -f *.elf
	rm -f *.list
	rm -f *.auto
	rm -f $(SDTARGET)

