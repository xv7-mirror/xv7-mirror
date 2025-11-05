include ../flags.mk

TARGET = _uptime
SRC = uptime.c

# We have to modify CFLAGS since ulib time.h conflicts with
# kernel time.h
CFLAGS = -fno-pic -static -fno-builtin -fno-strict-aliasing -O2 -Wall -MD -ggdb -m32 -fno-omit-frame-pointer -I../../ulib/
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)

all: $(TARGET)

$(TARGET): $(SRC) $(ULIB)
	$(CC) $(CFLAGS) -c $(SRC)
	$(LD) $(LDFLAGS) -N -e _start -Ttext 0 -o $(TARGET) $(basename $(SRC)).o $(ULIB)  # -I../../ulib/

clean:
	rm -f *.o *.d *.sym *.asm $(TARGET)
