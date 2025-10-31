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

# Cross-compiling (e.g., on Mac OS X)
TOOLPREFIX = i386-elf-

# Using native tools (e.g., on X86 Linux)
#TOOLPREFIX = 

# Try to infer the correct TOOLPREFIX if not set
ifndef TOOLPREFIX
TOOLPREFIX := $(shell if i386-jos-elf-objdump -i 2>&1 | grep '^elf32-i386$$' >/dev/null 2>&1; \
	then echo 'i386-jos-elf-'; \
	elif objdump -i 2>&1 | grep 'elf32-i386' >/dev/null 2>&1; \
	then echo ''; \
	else echo "***" 1>&2; \
	echo "*** Error: Couldn't find an i386-*-elf version of GCC/binutils." 1>&2; \
	echo "*** Is the directory with i386-jos-elf-gcc in your PATH?" 1>&2; \
	echo "*** If your i386-*-elf toolchain is installed with a command" 1>&2; \
	echo "*** prefix other than 'i386-jos-elf-', set your TOOLPREFIX" 1>&2; \
	echo "*** environment variable to that prefix and run 'make' again." 1>&2; \
	echo "*** To turn off this error, run 'gmake TOOLPREFIX= ...'." 1>&2; \
	echo "***" 1>&2; exit 1; fi)
endif

# If the makefile can't find QEMU, specify its path here
# QEMU = qemu-system-i386

# Try to infer the correct QEMU
ifndef QEMU
QEMU = $(shell if which qemu > /dev/null; \
	then echo qemu; exit; \
	elif which qemu-system-i386 > /dev/null; \
	then echo qemu-system-i386; exit; \
	elif which qemu-system-x86_64 > /dev/null; \
	then echo qemu-system-x86_64; exit; \
	else \
	qemu=/Applications/Q.app/Contents/MacOS/i386-softmmu.app/Contents/MacOS/i386-softmmu; \
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
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump
CFLAGS = -fno-pic -static -fno-builtin -fno-strict-aliasing -O2 -Wall -MD -ggdb -m32 -fno-omit-frame-pointer -Iincludes/generic -Iincludes/kernel
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)
ASFLAGS = -m32 -gdwarf-2 -Wa,-divide -Iincludes/generic -Iincludes/kernel
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
	tools/sign.pl bootblock

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

# kernelmemfs is a copy of kernel that maintains the
# disk image in memory instead of writing to a disk.
# This is not so useful for testing persistent storage or
# exploring disk buffering implementations, but it is
# great for testing the kernel on real hardware without
# needing a scratch disk.
MEMFSOBJS = $(filter-out ide.o,$(OBJS)) memide.o
kernelmemfs: $(MEMFSOBJS) entry.o entryother initcode kernel.ld fs.img
	$(LD) $(LDFLAGS) -T kernel.ld -o kernelmemfs kernel/entry.o  $(MEMFSOBJS) -b binary initcode entryother fs.img
	$(OBJDUMP) -S kernelmemfs > kernelmemfs.asm
	$(OBJDUMP) -t kernelmemfs | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > kernelmemfs.sym

tags: $(OBJS) entryother.S _init
	etags *.S *.c

kernel/vectors.S: tools/vectors.pl
	tools/vectors.pl > kernel/vectors.S

ULIB = ulib/ulib.o ulib/usys.o ulib/printf.o ulib/umalloc.o

_%: %.o $(ULIB)
	$(LD) $(LDFLAGS) -N -e main -Ttext 0 -o $@ $^
	$(OBJDUMP) -S $@ > $*.asm
	$(OBJDUMP) -t $@ | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $*.sym

_forktest: forktest.o $(ULIB)
	# forktest has less library code linked in - needs to be small
	# in order to be able to max out the proc table.
	$(LD) $(LDFLAGS) -N -e main -Ttext 0 -o _forktest forktest.o ulib/ulib.o ulib/usys.o
	$(OBJDUMP) -S _forktest > forktest.asm

mkfs: tools/mkfs.c includes/kernel/fs.h
	gcc -Werror -Wall -Itools/includes -o tools/mkfs tools/mkfs.c

games/%: games/%.o $(ULIB)
	$(LD) $(LDFLAGS) -N -e main -Ttext 0 -o $@ $^
	$(OBJDUMP) -S $@ > $*.asm
	$(OBJDUMP) -t $@ | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $@.sym

# Prevent deletion of intermediate files, e.g. cat.o, after first build, so
# that disk image changes after first build are persistent until clean.  More
# details:
# http://www.gnu.org/software/make/manual/html_node/Chained-Rules.html
.PRECIOUS: %.o

UPROGS=\
	userspace/bin/_yes\
	userspace/bin/_cat\
	userspace/bin/_echo\
	userspace/bin/_forktest\
	userspace/bin/_grep\
	userspace/bin/_init\
	userspace/bin/_kill\
	userspace/bin/_ln\
	userspace/bin/_ls\
	userspace/bin/_mkdir\
	userspace/bin/_rm\
	userspace/bin/_sh\
	userspace/bin/_stressfs\
	userspace/bin/_usertests\
	userspace/bin/_wc\
	userspace/bin/_zombie\
	userspace/bin/_touch\
	userspace/bin/_sleep\

# misc files
FILES=\
	userspace/etc/initlog.txt\

UPROG_LICENSES=\
	games/banner/banner.COPYING\

GAMES=\
	games/banner/_banner\

fs.img: mkfs $(UPROGS) $(GAMES)
	cp $(GAMES) userspace/bin/
	cp $(UPROG_LICENSES) userspace/bin/
	tools/mkfs fs.img userspace/bin/_* $(FILES) userspace/bin/*.COPYING

-include *.d

clean: 
	rm -f *.tex *.dvi *.idx *.aux *.log *.ind *.ilg \
	kernel/*.o kernel/dev/*.o kernel/dev/char/*.o kernel/*.d kernel/dev/*.d kernel/dev/char/*.d userspace/bin/*.o userspace/bin/*.d *.d *.o kernel/*.asm userspace/bin/*.asm *.asm kernel/*.sym userspace/bin/*.sym *.sym ulib/*.o ulib/*.d kernel/vectors.S bootblock entryother \
	initcode initcode.out xv7kernel xv7.img fs.img kernelmemfs \
	xv7memfs.img tools/mkfs .gdbinit \
	rm -f $(UPROGS) userspace/bin/_*
	rm -f $(GAMES) games/banner/*.d games/banner/*.sym games/banner/*.asm games/banner/*.o\
	rm -f userspace/bin/*.COPYING

# run in emulators

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
