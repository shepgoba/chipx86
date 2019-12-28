CC=gcc

main: main.c cpu.c ram.c loadrom.c display.c input.c
	$(CC) -g -o chipx86 main.c cpu.c ram.c loadrom.c display.c input.c -lSDL2main -lSDL2 -I.
