CC=gcc

main: main.c cpu.c ram.c loadrom.c display.c input.c
	$(CC) -O3 -o chipx86 main.c cpu.c ram.c loadrom.c display.c input.c -lSDL2main -lSDL2 -lSDL2_mixer -I. -arch x86_64
