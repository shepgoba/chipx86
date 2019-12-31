#ifndef DISPLAY_INCLUDE
#define DISPLAY_INCLUDE
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>


#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

typedef struct {
    SDL_Renderer *renderer;
    uint8_t frameBuf[SCREEN_HEIGHT][SCREEN_WIDTH];
} CHIP8_DISPLAY;
void clearScreen(CHIP8_DISPLAY *display);
void initDisplay(CHIP8_DISPLAY *display, SDL_Renderer *renderer);
void drawDisplay(CHIP8_DISPLAY *display);

#endif

