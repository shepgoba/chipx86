#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "cpu.h"
#include "ram.h"
#include "loadrom.h"
#include "display.h"
/*
Chip8-PC
(wip)

Chip8 interpreter for modern computers written in C

References:
http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#Annn
https://stackoverflow.com
*/
void cleanup(CHIP8_RAM *ram, CHIP8_DISPLAY *display) {
    free(ram->mem);
}

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
        return 1;
    } else {
        printf("loaded rom successfully\n");
    }
    CHIP8_CPU cpu;
    CHIP8_DISPLAY display;
    CHIP8_RAM ram;

    initRAM(&ram);
    initDisplay(&display);
    initCPU(&cpu, &ram, &display);
    char *windowTitle = "CHIP8-PC Development Version 1";
    SDL_Window *window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 320, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    //clearScreen(&display);
    //uint8_t fontData[16][5];
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
    /* 0x50+80 bytes of interpreter RAM are 0-F fontset */
    loadFontsetIntoRAM(fontData, &ram);
    loadProgramIntoRAM(romFile, &romFileSize, &ram);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    while (running) {
        SDL_Event e;
        while(SDL_PollEvent( &e ))
        {
            if( e.type == SDL_QUIT )
            {
                running = 0;
            }
        }
        executeInstructions(&cpu);
        SDL_RenderClear(renderer);
        drawDisplay(&display, renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(1000/targetFrameRate);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    printf("exiting\n");
    cleanup(&ram, &display);
    SDL_Quit();
    return 0;
}
