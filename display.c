#include "display.h"
void initDisplay(CHIP8_DISPLAY *display) {
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        for (int j = 0; j < SCREEN_WIDTH; j++) {
            display->frameBuf[i][j] = 0;
        }
    }
}
void clearScreen(CHIP8_DISPLAY *display) {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            display->frameBuf[y][x] = 0;
        }
    }
}
void drawDisplay(CHIP8_DISPLAY *display, SDL_Renderer *renderer) {
    SDL_Rect rect;
    rect.w = 10;
    rect.h = 10;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            if (display->frameBuf[y][x]) {
                rect.x = 10*x;
                rect.y = 10*y;
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}
