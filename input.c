#include "input.h"
void handleKeypress(SDL_Event *e, CHIP8_CPU *cpu) {
    int key = e->key.keysym.sym;
    if (e->type == SDL_KEYDOWN) {
        switch (key) {
            case SDLK_q:
                cpu->KEY[0] = 1;
            break;
            case SDLK_w:
                cpu->KEY[1] = 1;
            break;
            case SDLK_e:
                cpu->KEY[2] = 1;
            break;
            case SDLK_r:
                cpu->KEY[3] = 1;
            break;
            case SDLK_a:
                cpu->KEY[4] = 1;
            break;
            case SDLK_s:
                cpu->KEY[5] = 1;
            break;
            case SDLK_d:
                cpu->KEY[6] = 1;
            break;
            case SDLK_f:
                cpu->KEY[7] = 1;
            break;
            case SDLK_u:
                cpu->KEY[8] = 1;
            break;
            case SDLK_i:
                cpu->KEY[9] = 1;
            break;
            case SDLK_o:
                cpu->KEY[10] = 1;
            break;
            case SDLK_p:
                cpu->KEY[11] = 1;
            break;
            case SDLK_j:
                cpu->KEY[12] = 1;
            break;
            case SDLK_k:
                cpu->KEY[13] = 1;
            break;
            case SDLK_l:
                cpu->KEY[14] = 1;
            break;
            case SDLK_SEMICOLON:
                cpu->KEY[15] = 1;
            break;
        }
    } else if (e->type == SDL_KEYUP) {
        switch (key) {
            case SDLK_q:
                cpu->KEY[0] = 0;
            break;
            case SDLK_w:
                cpu->KEY[1] = 0;
            break;
            case SDLK_e:
                cpu->KEY[2] = 0;
            break;
            case SDLK_r:
                cpu->KEY[3] = 0;
            break;
            case SDLK_a:
                cpu->KEY[4] = 0;
            break;
            case SDLK_s:
                cpu->KEY[5] = 0;
            break;
            case SDLK_d:
                cpu->KEY[6] = 0;
            break;
            case SDLK_f:
                cpu->KEY[7] = 0;
            break;
            case SDLK_u:
                cpu->KEY[8] = 0;
            break;
            case SDLK_i:
                cpu->KEY[9] = 0;
            break;
            case SDLK_o:
                cpu->KEY[10] = 0;
            break;
            case SDLK_p:
                cpu->KEY[11] = 0;
            break;
            case SDLK_j:
                cpu->KEY[12] = 0;
            break;
            case SDLK_k:
                cpu->KEY[13] = 0;
            break;
            case SDLK_l:
                cpu->KEY[14] = 0;
            break;
            case SDLK_SEMICOLON:
                cpu->KEY[15] = 0;
            break;
        }
    } else {

    }
}
