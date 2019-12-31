#ifndef CPU_INCLUDE
#define CPU_INCLUDE
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_audio.h>
#include "ram.h"
#include "display.h"
typedef struct {
    /* Registers */
    uint8_t reg[16]; // General purpose
    uint8_t *VF;
    uint8_t DT, ST; // Delay timer, sound timer
    uint16_t SP; //Stack pointer (points to top of the stack)
    uint16_t PC; // Program counter
    uint16_t I; //Multi use register
    uint16_t STACK[16]; // Stack

    uint16_t KEY; // Hold bits for each key
    // Abstractions for interpreter
    CHIP8_RAM *ramPtr;
    CHIP8_DISPLAY *displayPtr;
    SDL_AudioSpec freqSound;
} CHIP8_CPU;
void initCPU(CHIP8_CPU *cpu, CHIP8_RAM *ram, CHIP8_DISPLAY *display);
void executeInstructions(CHIP8_CPU *cpu);
void handleTimers(CHIP8_CPU *cpu);
void debugDrawKeys(CHIP8_CPU *cpu, CHIP8_DISPLAY *display);
#endif
