BUILD := ./build
SRC := ./src

CC := gcc

ENTRYPOINT := 0x10000

CFLAGS := -m32	# i386 program, not amd64
CFLAGS += -fno-builtin -nostdinc -nostdlib	# No standard library
CFLAGS += -fno-pic -fno-pie	# No position-independent code and executable
CFLAGS += -fno-stack-protector
CFLAGS := $(strip $(CFLAGS))

DEBUG := -g -DDEBUG
INCLUDE := -I$(SRC)/include

.PHONY: bochs
bochs: $(BUILD)/master_i386.img
	bochs -q -f bochsrc

.PHONY: bochs-gdb
bochs-gdb: $(BUILD)/master_i386.img
	bochs-gdb -q -f bochsrc.gdb

.PHONY: qemui386
qemui386: $(BUILD)/master_i386.img
	qemu-system-i386 \
		-m 32M \
		-boot c \
		-hda $< 

.PHONY: qemui386-gdb
qemui386-gdb: $(BUILD)/master_i386.img
	qemu-system-i386 \
		-s -S \
		-m 32M \
		-boot c \
		-hda $< 

.PHONY: clean
clean:
	- rm -r $(BUILD)/*

$(BUILD)/boot/%.bin: $(SRC)/boot/%.asm
	mkdir -p $(dir $@)
	nasm -f bin $< -o $@

$(BUILD)/kernel/%.o: $(SRC)/kernel/%.asm
	mkdir -p $(dir $@)
	nasm $(DEBUG) -f elf32 $< -o $@

$(BUILD)/kernel/%.o: $(SRC)/kernel/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@



$(BUILD)/kernel_i386.elf: \
	$(BUILD)/kernel/arch/i386/start.o \
	$(BUILD)/kernel/main.o \
	$(BUILD)/kernel/console.o \
	$(BUILD)/kernel/memutil.o \
	$(BUILD)/kernel/stdioimpl.o \
	$(BUILD)/kernel/i64arith.o \
	$(BUILD)/kernel/assert.o \
	$(BUILD)/kernel/arch/i386/gdt.o \
	$(BUILD)/kernel/arch/i386/idt.o \
	$(BUILD)/kernel/arch/i386/interrupt_handler.o

	ld -m elf_i386 -static $^ -o $@ -Ttext $(ENTRYPOINT)

$(BUILD)/system_i386.bin: $(BUILD)/kernel_i386.elf
	objcopy -O binary $< $@

$(BUILD)/system_i386.map: $(BUILD)/kernel_i386.elf
	nm $< | sort > $@

$(BUILD)/master_i386.img: \
	$(BUILD)/boot/boot.bin \
	$(BUILD)/boot/loader.bin \
	$(BUILD)/system_i386.bin \
	$(BUILD)/system_i386.map

	yes | bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat $@
	dd if=$(BUILD)/boot/boot.bin of=$@ bs=512 count=1 conv=notrunc
	dd if=$(BUILD)/boot/loader.bin of=$@ bs=512 count=4 seek=2 conv=notrunc
	dd if=$(BUILD)/system_i386.bin of=$@ bs=512 count=200 seek=10 conv=notrunc
