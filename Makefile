ARMGNU = arm-none-eabi
SHELL = /bin/sh

TARGET = kernel7.bin
SDTARGET = /Volumes/boot/kernel7.img
DEPS = *.h
DIRS = shell app1 app2

COPS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding

all : $(TARGET) 
	@for i in $(DIRS) ; do ( cd $$i ; make $(MAKECMDGOALS) ) ; done

cp : $(TARGET) 
	cp $(TARGET) $(SDTARGET)
	@for i in $(DIRS) ; do ( cd $$i ; make $(MAKECMDGOALS) ) ; done

%.o : %.s
	$(ARMGNU)-as $< -o $@

%.o : %.c $(DEPS)
	$(ARMGNU)-gcc $(COPS) -c $< -o $@

OBJECTS := $(patsubst %.s,%.o,$(wildcard *.s)) $(patsubst %.c,%.o,$(wildcard *.c))

$(TARGET) : $(OBJECTS) 
	make kversion
	$(ARMGNU)-ld $(OBJECTS) -T memmap -o kernel7.elf
	$(ARMGNU)-objdump -D kernel7.elf > kernel7.list
	$(ARMGNU)-objcopy kernel7.elf -O binary $(TARGET)

kversion :
	echo char kversion\[\] = \"Kernel version \[`basename \`pwd\``\, `date`\]\"\; > kversion.c
	$(ARMGNU)-gcc $(COPS) -c kversion.c -o kversion.o

clean :
	rm -f *.o
	rm -f *.bin
	rm -f *.elf
	rm -f *.list
	rm -f *.auto
	@for i in $(DIRS) ; do ( cd $$i ; make $(MAKECMDGOALS) ) ; done


