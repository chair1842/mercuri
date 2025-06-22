TARGET = kernel

# Source directories
KERNEL_DIR = kernel
DRIVER_DIR = driver
BOOTLOADER_DIR = bootloader
INCLUDE_DIR = include

# Source files
KERNEL_SRCS    = $(wildcard $(KERNEL_DIR)/*.c) $(wildcard $(KERNEL_DIR)/memory/*.c) $(wildcard $(KERNEL_DIR)/interrupts/*.c)
DRIVER_SRCS    = $(wildcard $(DRIVER_DIR)/*.c)
BOOTLOADER_SRCS = $(wildcard $(BOOTLOADER_DIR)/*.s)
ASM_SRCS       = $(wildcard $(KERNEL_DIR)/*.S) $(wildcard $(KERNEL_DIR)/interrupts/*.asm)

# Object files
KERNEL_OBJS     = $(KERNEL_SRCS:.c=.o)
DRIVER_OBJS     = $(DRIVER_SRCS:.c=.o)
BOOTLOADER_OBJS = $(BOOTLOADER_SRCS:.s=.o)
ASM_OBJS        = $(ASM_SRCS:.asm=.o)

OBJS = $(ASM_OBJS) $(KERNEL_OBJS) $(DRIVER_OBJS) $(BOOTLOADER_OBJS)

# Toolchain
CC = i686-elf-gcc
AS = i686-elf-as
LD = i686-elf-ld

CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I$(INCLUDE_DIR)
LDFLAGS = -T linker.ld -nostdlib

# Targets
all: $(TARGET).bin

kernel/interrupts/isr_stubs.o: kernel/interrupts/isr_stubs.asm
	nasm -f elf $< -o $@
	
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(AS) $< -o $@

$(TARGET).bin: $(OBJS)
	echo "Linking $^" 
	$(LD) $(LDFLAGS) -o $@ $^

run: $(TARGET).bin
	qemu-system-i386 -kernel $(TARGET).bin

clean:
	rm -f $(OBJS) $(TARGET).bin

auto: clean all run

debug:
	echo $(ASM_OBJS)
	echo $(ASM_)