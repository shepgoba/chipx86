CC=gcc
CFLAGS=-I.

main: main.c cpu.c ram.c loadrom.c display.c input.c
	$(CC) -g -o chip8 main.c cpu.c ram.c loadrom.c display.c input.c -lSDL2main -lSDL2 -I.
