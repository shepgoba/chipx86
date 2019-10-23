#ifndef CPU_INCLUDE
#define CPU_INCLUDE
#include <stdio.h>
#include "ram.h"
#include "display.h"
typedef struct {
    /* Registers */
    uint8_t *regptr[16];
    uint8_t V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, VA, VB, VC, VD, VE, VF; // General purpose
    uint8_t DT, ST; // Delay timer, sound timer
    uint16_t SP; //Stack pointer (points to top of the stack)
    uint16_t PC; // Program counter
    uint16_t I;
    uint16_t STACK[24]; // Stack
    uint8_t KEY[16];
    CHIP8_RAM *ramPtr;
    CHIP8_DISPLAY *displayPtr;
} CHIP8_CPU;
void initCPU(CHIP8_CPU *cpu, CHIP8_RAM *ram, CHIP8_DISPLAY *display);
void executeInstructions(CHIP8_CPU *cpu);
#endif
