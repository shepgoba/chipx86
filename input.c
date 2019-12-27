#include "input.h"
void handleKeypress(SDL_Event *e, CHIP8_CPU *cpu) {
    int key = e->key.keysym.sym;
    printf("kek");
    switch (key) {
        case SDLK_q:
            cpu->KEY[0] = 1;
        break;
        case SDLK_w:
            cpu->KEY[1] = 1;
        case SDLK_e:
            cpu->KEY[2] = 1;
        break;
    }
}
