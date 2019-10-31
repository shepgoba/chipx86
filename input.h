#ifndef INPUT_INCLUDE
#define INPUT_INCLUDE
#include <stdio.h>
#include <SDL2/SDL.h>
#include "cpu.h"
void handleKeypress(SDL_Event *e, CHIP8_CPU *cpu);
#endif
