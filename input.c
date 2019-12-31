#include "input.h"
void handleKeypress(SDL_Event *e, CHIP8_CPU *cpu) {
    int key = e->key.keysym.sym;
    static int keys[] = {SDLK_x, SDLK_1, SDLK_2, SDLK_3, SDLK_q, SDLK_w, SDLK_e, SDLK_a, SDLK_s, SDLK_d, SDLK_z, SDLK_c, SDLK_4, SDLK_r, SDLK_f, SDLK_v};
    uint8_t keyStatus = e->type == SDL_KEYDOWN ? 1 : 0;
    for (int i = 0; i < 0xF; i++) {
        if (key == keys[i])
            cpu->KEY ^= (-keyStatus ^ cpu->KEY) & (1UL << i);
    }
}
