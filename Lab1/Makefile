CC = gcc
CFLAGS = -std=c99

TARGET = assemble

.PHONY: all clean

all: $(TARGET)

$(TARGET): assembler.c
	$(CC) $(CFLAGS) -o $(TARGET) assembler.c

%:
	./assemble ./asm/$*.asm ./obj/$*_my.obj
	./assembler.linux ./asm/$*.asm ./obj/$*_gd.obj
	./obj/compare ./obj/$*

clean: 
	rm -f $(TARGET) ./obj/*_my.obj ./obj/*_gd.obj


