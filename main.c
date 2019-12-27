#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include "cpu.h"
#include "ram.h"
#include "loadrom.h"
#include "display.h"
#include "input.h"

/*
Chip8-PC
(wip)

Chip8 interpreter for modern computers written in C

References:
http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#Annn
https://stackoverflow.com
*/


int main(int argc, char *argv[]) {
    uint8_t running = 1;
    const int targetFrameRate = 60;
    SDL_Init(SDL_INIT_EVERYTHING);
    srand(time(NULL));
    if (argc < 2) {
        puts("Usage: ./chip8 <ROM file>");
        return 1;
    }
    char *romFileName = argv[1];

    FILE *romFile;
    uint16_t romFileSize;

    if (!(romFile = openROM(romFileName, &romFileSize))) {
        exit(-1);
    } else {
        printf("loaded rom successfully\n");
    }

    CHIP8_CPU cpu;
    CHIP8_DISPLAY display;
    CHIP8_RAM ram;

    char *windowTitle = "CHIP8-PC Development Version 1";
    SDL_Window *window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 320, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    
    initRAM(&ram);
    initDisplay(&display, renderer);
    initCPU(&cpu, &ram, &display);
    uint8_t fontData[16][5] = {
        {0xF0, 0x90, 0x90, 0x90, 0xF0}, //0
        {0x20, 0x60, 0x20, 0x20, 0x70}, //1
        {0xF0, 0x10, 0xF0, 0x80, 0xF0}, //2
        {0xF0, 0x10, 0xF0, 0x10, 0xF0}, //3
        {0x90, 0x90, 0xF0, 0x10, 0x10}, //4
        {0xF0, 0x80, 0xF0, 0x10, 0xF0}, //5
        {0xF0, 0x80, 0xF0, 0x90, 0xF0}, //6
        {0xF0, 0x10, 0x20, 0x40, 0x40}, //7
        {0xF0, 0x90, 0xF0, 0x90, 0xF0}, //8
        {0xF0, 0x90, 0xF0, 0x10, 0xF0}, //9
        {0xF0, 0x90, 0xF0, 0x90, 0x90}, //A
        {0xE0, 0x90, 0xE0, 0x90, 0xE0}, //B
        {0xF0, 0x80, 0x80, 0x80, 0xF0}, //C
        {0xE0, 0x90, 0x90, 0x90, 0xE0}, //D
        {0xF0, 0x80, 0xF0, 0x80, 0xF0}, //E
        {0xF0, 0x80, 0xF0, 0x80, 0x80}, //F
    };
    /* 0x50+80 bytes of interpreter RAM are for the 0-F fontset */
    loadFontsetIntoRAM(fontData, &ram);
    loadProgramIntoRAM(romFile, &romFileSize, &ram);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    FILE *dump = fopen("debug-instruction-dump.txt", "wb+");
    FILE *dump2 = fopen("debug-registers.txt", "w+");
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            } else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
                handleKeypress(&e, &cpu);
            }
        }
        executeInstructions(&cpu, dump, dump2);
        delayTimer(&cpu);
        SDL_Delay(1000 / targetFrameRate);
    }
    fclose(dump);
    fclose(dump2);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    printf("exiting\n");
    SDL_Quit();
    return 0;
}
