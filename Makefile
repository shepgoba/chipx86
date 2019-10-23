CC=gcc
CFLAGS=-I.

main: main.c cpu.c ram.c loadrom.c display.c
	$(CC) -o chip8 main.c cpu.c ram.c loadrom.c display.c -lSDL2main -lSDL2 -I.
