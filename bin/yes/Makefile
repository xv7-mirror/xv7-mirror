include ../flags.mk

TARGET = _yes
SRC = yes.c

all: $(TARGET)

$(TARGET): $(SRC) $(ULIB)
	$(CC) $(CFLAGS) -c $(SRC)
	$(LD) $(LDFLAGS) -N -e _start -Ttext 0 -o $(TARGET) $(basename $(SRC)).o $(ULIB)

clean:
	rm -f *.o *.d *.sym *.asm $(TARGET)
