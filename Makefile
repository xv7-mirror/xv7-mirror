OBJS = \
	kernel/bio.o\
	kernel/dev/char/console.o\
	kernel/exec.o\
	kernel/file.o\
	kernel/fs.o\
	kernel/dev/ide.o\
	kernel/ioapic.o\
	kernel/kalloc.o\
	kernel/dev/char/kbd.o\
	kernel/lapic.o\
	kernel/log.o\
	kernel/main.o\
	kernel/mp.o\
	kernel/picirq.o\
	kernel/pipe.o\
	kernel/proc.o\
	kernel/sleeplock.o\
	kernel/spinlock.o\
	kernel/string.o\
	kernel/swtch.o\
	kernel/syscall.o\
	kernel/sysfile.o\
	kernel/sysproc.o\
	kernel/trapasm.o\
	kernel/trap.o\
	kernel/dev/char/uart.o\
	kernel/vectors.o\
	kernel/vm.o\

# By default, we should cross-compile to i386-elf.
TOOLPREFIX = i386-elf-

# Try to infer the correct QEMU
ifndef QEMU
QEMU = $(shell if which qemu > /dev/null; \
	then echo qemu; exit; \
	elif which qemu-system-i386 > /dev/null; \
	then echo qemu-system-i386; exit; \
	elif which qemu-system-x86_64 > /dev/null; \
	then echo qemu-system-x86_64; exit; \
	if test -x $$qemu; then echo $$qemu; exit; fi; fi; \
	echo "***" 1>&2; \
	echo "*** Error: Couldn't find a working QEMU executable." 1>&2; \
	echo "*** Is the directory containing the qemu binary in your PATH" 1>&2; \
	echo "*** or have you tried setting the QEMU variable in Makefile?" 1>&2; \
	echo "***" 1>&2; exit 1)
endif

CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)gas
LD = $(TOOLPREFIX)ld
AR = $(TOOLPREFIX)ar
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump
CFLAGS = -fno-pic -static -fno-builtin -fno-strict-aliasing -O2 -Wall -MD -ggdb -m32 -fno-omit-frame-pointer -Iincludes/generic -Iincludes/kernel -Iulib/
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)
ASFLAGS = -m32 -gdwarf-2 -Wa,-divide -Iincludes/generic -Iincludes/kernel -Iulib/
# FreeBSD ld wants ``elf_i386_fbsd''
LDFLAGS += -m $(shell $(LD) -V | grep elf_i386 2>/dev/null | head -n 1)

# Disable PIE when possible (for Ubuntu 16.10 toolchain)
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]no-pie'),)
CFLAGS += -fno-pie -no-pie
endif
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]nopie'),)
CFLAGS += -fno-pie -nopie
endif

xv7.img: bootblock xv7kernel
	dd if=/dev/zero of=xv7.img count=10000
	dd if=bootblock of=xv7.img conv=notrunc
	dd if=xv7kernel of=xv7.img seek=1 conv=notrunc

xv7memfs.img: bootblock kernelmemfs
	dd if=/dev/zero of=xv7memfs.img count=10000
	dd if=bootblock of=xv7memfs.img conv=notrunc
	dd if=kernelmemfs of=xv7memfs.img seek=1 conv=notrunc

bootblock: bootldr/bootasm.S bootldr/bootmain.c
	$(CC) $(CFLAGS) -fno-pic -O -nostdinc -Iincludes/generic -c bootldr/bootmain.c
	$(CC) $(CFLAGS) -fno-pic -nostdinc -Iincludes/generic -c bootldr/bootasm.S
	$(LD) $(LDFLAGS) -N -e start -Ttext 0x7C00 -o bootblock.o bootasm.o bootmain.o
	$(OBJDUMP) -S bootblock.o > bootblock.asm
	$(OBJCOPY) -S -O binary -j .text bootblock.o bootblock
	tools/sign.sh bootblock

entryother: kernel/entryother.S
	$(CC) $(CFLAGS) -fno-pic -nostdinc -I. -c kernel/entryother.S
	$(LD) $(LDFLAGS) -N -e start -Ttext 0x7000 -o bootblockother.o entryother.o
	$(OBJCOPY) -S -O binary -j .text bootblockother.o entryother
	$(OBJDUMP) -S bootblockother.o > entryother.asm

initcode: kernel/initcode.S
	$(CC) $(CFLAGS) -nostdinc -I. -c kernel/initcode.S
	$(LD) $(LDFLAGS) -N -e start -Ttext 0 -o initcode.out initcode.o
	$(OBJCOPY) -S -O binary initcode.out initcode
	$(OBJDUMP) -S initcode.o > initcode.asm

xv7kernel: $(OBJS) kernel/entry.o entryother initcode kernel.ld
	$(LD) $(LDFLAGS) -T kernel.ld -o xv7kernel kernel/entry.o $(OBJS) -b binary initcode entryother
	$(OBJDUMP) -S xv7kernel > kernel.asm
	$(OBJDUMP) -t xv7kernel | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > kernel.sym

MEMFSOBJS = $(filter-out kernel/dev/ide.o,$(OBJS)) kernel/dev/memide.o
kernelmemfs: $(MEMFSOBJS) kernel/entry.o entryother initcode kernel.ld fs.img
	$(LD) $(LDFLAGS) -T kernel.ld -o kernelmemfs kernel/entry.o  $(MEMFSOBJS) -b binary initcode entryother fs.img
	$(OBJDUMP) -S kernelmemfs > kernelmemfs.asm
	$(OBJDUMP) -t kernelmemfs | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > kernelmemfs.sym

kernel/vectors.S: tools/vectors.sh
	tools/vectors.sh > kernel/vectors.S

ULIB = ulib/crt0.o ulib/ulib.o ulib/usys.o ulib/printf.o ulib/umalloc.o

#
# Build static archive for libc
#
ULIB_OBJS = ulib/crt0.o ulib/ulib.o ulib/usys.o ulib/printf.o ulib/umalloc.o ulib/files.o ulib/err.o ulib/dirent.o ulib/progname.o ulib/errno.o

copy-headers:
	rm -rf userspace/include
	mkdir userspace/include/
	mkdir userspace/include/sys/
	cp ulib/*.h userspace/include/
	cp ulib/sys/*.h userspace/include/sys/

userspace/lib/libc.a: $(ULIB_OBJS)
	rm -rf userspace/lib
	mkdir userspace/lib
	$(AR) rcs userspace/lib/libc.a $(ULIB_OBJS)

ULIB = userspace/lib/libc.a

mkfs: tools/mkfs.c includes/kernel/fs.h
	gcc -Werror -Wall -Itools/includes -o tools/mkfs tools/mkfs.c

# misc files
FILES=\
	userspace/etc/README\
	userspace/lib/libc.a

fs.img: $(ULIB) mkfs copy-headers
	rm -rf userspace/bin
	mkdir userspace/bin
	$(MAKE) -C bin all
	$(MAKE) -C games all
	tools/mkfs fs.img userspace/bin/_* $(FILES) userspace/bin/*.COPYING

clean: 
	rm -f *.tex *.dvi *.idx *.aux *.log *.ind *.ilg \
	kernel/*.o kernel/dev/*.o kernel/dev/char/*.o kernel/*.d kernel/dev/*.d kernel/dev/char/*.d *.d *.o kernel/*.asm *.asm kernel/*.sym *.sym ulib/*.o ulib/*.d kernel/vectors.S bootblock entryother \
	initcode initcode.out xv7kernel xv7.img fs.img kernelmemfs \
	xv7memfs.img tools/mkfs .gdbinit \
	rm -rf userspace/bin userspace/lib userspace/include
	$(MAKE) -C bin clean
	$(MAKE) -C games clean

# run in QEMU

# try to generate a unique GDB port
GDBPORT = $(shell expr `id -u` % 5000 + 25000)
# QEMU's gdb stub command line changed in 0.11
QEMUGDB = $(shell if $(QEMU) -help | grep -q '^-gdb'; \
	then echo "-gdb tcp::$(GDBPORT)"; \
	else echo "-s -p $(GDBPORT)"; fi)
ifndef CPUS
CPUS := 2
endif
QEMUOPTS = -drive file=fs.img,index=1,media=disk,format=raw -drive file=xv7.img,index=0,media=disk,format=raw -smp $(CPUS) -m 512 $(QEMUEXTRA)

qemu: fs.img xv7.img
	$(QEMU) -serial mon:stdio $(QEMUOPTS)

qemu-memfs: xv7memfs.img
	$(QEMU) -drive file=xv7memfs.img,index=0,media=disk,format=raw -smp $(CPUS) -m 256

qemu-nox: fs.img xv7.img
	$(QEMU) -nographic $(QEMUOPTS)

.gdbinit: .gdbinit.tmpl
	sed "s/localhost:1234/localhost:$(GDBPORT)/" < $^ > $@

qemu-gdb: fs.img xv7.img .gdbinit
	@echo "*** Now run 'gdb'." 1>&2
	$(QEMU) -serial mon:stdio $(QEMUOPTS) -S $(QEMUGDB)

qemu-nox-gdb: fs.img xv7.img .gdbinit
	@echo "*** Now run 'gdb'." 1>&2
	$(QEMU) -nographic $(QEMUOPTS) -S $(QEMUGDB)

# Grab all .c files
FSRC := $(shell find . -name '*.c' -o -name '*.h')

# Format all code
# this could take a bit of time
format:
	@clang-format -i $(FSRC)
	@echo "Done"
