#include "display.h"
void initDisplay(CHIP8_DISPLAY *display, SDL_Renderer *renderer) {
    display->renderer = renderer;
    clearScreen(display);
}
void clearScreen(CHIP8_DISPLAY *display) {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            display->frameBuf[y][x] = 0;//0x1010101010101010;
        }
    }
}

void drawDisplay(CHIP8_DISPLAY *display) {
    static SDL_Rect rect;
    rect.w = 10;
    rect.h = 10;

    SDL_SetRenderDrawColor(display->renderer, 255, 255, 255, 255);
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            if (display->frameBuf[y][x]) {
                rect.x = 10 * x;
                rect.y = 10 * y;
                SDL_RenderFillRect(display->renderer, &rect);
            }
            //printf("(%i, %i)\n", rect.x, x);
            #ifdef DEBUG
            debugDrawKeys(display);
            #endif
        }
    }
    SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);
}