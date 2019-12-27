#ifndef INSTRUCTIONS_INCLUDE
#define INSTRUCTIONS_INCLUDE
#include "cpu.h"
void CLS(CHIP8_CPU *cpu);
void RET(CHIP8_CPU *cpu);
void JP(CHIP8_CPU *cpu);
void CALL(CHIP8_CPU *cpu);
void SEBYTE(CHIP8_CPU *cpu);
#endif
