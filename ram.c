#include "ram.h"
void initRAM(CHIP8_RAM *ram) {
    for (int i = 0; i < RAM_SIZE; i++) {
        ram->mem[i] = 0;
    }
    printf("oof");
}
void loadProgramIntoRAM(FILE *romFile, uint16_t *romFileSize, CHIP8_RAM *ram) {
    for (int i = 0x200; i < *romFileSize + 0x200; i++) {
        fread(ram->mem + i, sizeof(uint8_t), 1, romFile);
    }
}

void loadFontsetIntoRAM(uint8_t fontset[16][5], CHIP8_RAM *ram) {
    uint8_t location = 0x50;
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 5; j++) {
            ram->mem[location] = fontset[i][j];
            location++;
        }
    }
}
