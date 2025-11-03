include ../flags.mk

TARGET = _init
SRC = init.c

all: $(TARGET)

$(TARGET): $(SRC) $(ULIB)
	$(CC) $(CFLAGS) -c $(SRC)
	$(LD) $(LDFLAGS) -N -e _start -Ttext 0 -o $(TARGET) $(basename $(SRC)).o $(ULIB)  -I../../ulib/

clean:
	rm -f *.o *.d *.sym *.asm $(TARGET)
