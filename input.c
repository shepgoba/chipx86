#include "input.h"
void handleKeypress(SDL_Event *e, CHIP8_CPU *cpu) {
    int key = e->key.keysym.sym;
    for (int i = 0; i < 10; i++) {
        if (key == SDLK_0 + i) {
            uint8_t setOn = 0;
            if (e->type == SDL_KEYDOWN)
                setOn = 1;
            cpu->KEY[i] = setOn;
            #ifdef DEBUG
            printf(e->type == SDL_KEYDOWN ? "KEYDOWN:  %i\n" : "KEYUP: %i\n", i);
            #endif
        }
    }
}
