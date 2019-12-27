#ifndef RAM_INCLUDE
#define RAM_INCLUDE
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define RAM_SIZE 4096
typedef struct {
    uint8_t mem[RAM_SIZE];
} CHIP8_RAM;
void loadProgramIntoRAM(FILE *romFile, uint16_t *romFileSize, CHIP8_RAM *ram);
void loadFontsetIntoRAM(uint8_t fontset[16][5], CHIP8_RAM *ram);
void initRAM(CHIP8_RAM *ram);
#endif
