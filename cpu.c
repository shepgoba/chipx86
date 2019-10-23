#include <cpu.h>
void initCPU(CHIP8_CPU *cpu, CHIP8_RAM *ram, CHIP8_DISPLAY *display) {
    cpu->regptr[0] = &cpu->V0;
    cpu->regptr[1] = &cpu->V1;
    cpu->regptr[2] = &cpu->V2;
    cpu->regptr[3] = &cpu->V3;
    cpu->regptr[4] = &cpu->V4;
    cpu->regptr[5] = &cpu->V5;
    cpu->regptr[6] = &cpu->V6;
    cpu->regptr[7] = &cpu->V7;
    cpu->regptr[8] = &cpu->V8;
    cpu->regptr[9] = &cpu->V9;
    cpu->regptr[10] = &cpu->VA;
    cpu->regptr[11] = &cpu->VB;
    cpu->regptr[12] = &cpu->VC;
    cpu->regptr[13] = &cpu->VD;
    cpu->regptr[14] = &cpu->VE;
    cpu->regptr[15] = &cpu->VF;
    cpu->SP = 0;
    cpu->PC = 0x200;
    cpu->ramPtr = ram;
    cpu->displayPtr = display;
}
void displaySprite(CHIP8_CPU *cpu, uint8_t x, uint8_t y, uint8_t spriteSize, uint8_t *sprite) {
    int oldX = x;
    int oldY = y;
    for (; y < oldY + spriteSize; y++) {
        for (; x < oldX + 8; x++) {
            uint8_t bitStatus = (*(sprite + (y - oldY)) >> (8 - (x - oldX))) & 1;
            if (cpu->displayPtr->frameBuf[y][x] & bitStatus) {
                cpu->VF = 1;
            } else {
                cpu->VF = 0;
            }
            cpu->displayPtr->frameBuf[y][x] = cpu->displayPtr->frameBuf[y][x] ^ bitStatus;
        }
        x = oldX;
    }
}
void executeInstructions(CHIP8_CPU *cpu) {
    for (int cycles = 0; cycles <= 9; cycles++)
    {
        uint8_t incrementPC = 1;

        uint8_t *ram = cpu->ramPtr->mem;
        uint16_t opcodeOffset = cpu->PC;
        uint16_t opCode = (ram[opcodeOffset] << 8) + ram[opcodeOffset + 1];
        uint8_t instructionHeader = (opCode >> 12) & 0xf;
        uint8_t instructionArg1 = (opCode >> 8) & 0xf;
        uint8_t instructionArg2 = (opCode >> 4) & 0xf;
        uint8_t instructionArg3 = opCode & 0xf;
        uint16_t instructionArg1to3 = opCode & 0xfff;
        uint8_t instructionArg2to3 = opCode & 0xff;
        printf("Will be executing instruction: 0x%x, PC = 0x%x\n", opCode, cpu->PC);
        if (instructionHeader == 0x0) {
            if (instructionArg3 == 0x0) { //CLS
                clearScreen(cpu->displayPtr);
            } else if (instructionArg3 == 0xE) { //RET
                //TODO
            }
        } else if (instructionHeader == 0x1) { //JMP
            cpu->PC = instructionArg1to3;
            incrementPC = 0;
        } else if (instructionHeader == 0x2) {
            //increment stack pointer, put the current PC on top of stack, PC Set to 2nnn
            cpu->STACK[cpu->SP] = cpu->PC;
            cpu->SP++;
            cpu->PC = instructionArg1to3;
        } else if (instructionHeader == 0x3) {
            printf("V%x = %x, arg2to3 = %x\n", instructionArg1, *(cpu->regptr[instructionArg1]), instructionArg2to3);
            if (*(cpu->regptr[instructionArg1]) == instructionArg2to3) {
                //printf("V%x = %x, arg2to3 = %x\n", instructionArg1, *(cpu->regptr[instructionArg1]), instructionArg2to3);
                cpu->PC += 2;
            }
        } else if (instructionHeader == 0x4) {
            if (*(cpu->regptr[instructionArg1]) != instructionArg2to3) {
                cpu->PC += 2;
            }
        } else if (instructionHeader == 0x5) {
            if (*(cpu->regptr[instructionArg1]) == *(cpu->regptr[instructionArg2])) {
                cpu->PC += 2;
            }
        } else if (instructionHeader == 0x6) {
            *(cpu->regptr[instructionArg1]) = instructionArg2to3;
        } else if (instructionHeader == 0x7) {
            *(cpu->regptr[instructionArg1]) += instructionArg2to3;
        } else if (instructionHeader == 0x8) {
            if (instructionArg3 == 0x0) {
                *(cpu->regptr[instructionArg1]) = *(cpu->regptr[instructionArg2]);
            } else if (instructionArg3 == 0x1) {
                *(cpu->regptr[instructionArg1]) = *(cpu->regptr[instructionArg1]) | *(cpu->regptr[instructionArg2]);
            } else if (instructionArg3 == 0x2) {
                *(cpu->regptr[instructionArg1]) = *(cpu->regptr[instructionArg1]) & *(cpu->regptr[instructionArg2]);
            } else if (instructionArg3 == 0x3) {
                *(cpu->regptr[instructionArg1]) = *(cpu->regptr[instructionArg1]) ^ *(cpu->regptr[instructionArg2]);
            } else if (instructionArg3 == 0x4) {
                //The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0. Only the lowest 8 bits of the result are kept, and stored in Vx.
                *(cpu->regptr[instructionArg1]) += *(cpu->regptr[instructionArg2]);
                if (*(cpu->regptr[instructionArg1]) + *(cpu->regptr[instructionArg2]) != (*(cpu->regptr[instructionArg1]) + *(cpu->regptr[instructionArg3]) & 0xFF)) {
                    cpu->VF = 1;
                } else {
                    cpu->VF = 0;
                }
            } else if (instructionArg3 == 0x5) {
                *(cpu->regptr[instructionArg1]) -= *(cpu->regptr[instructionArg2]);
                if (*(cpu->regptr[instructionArg1]) > *(cpu->regptr[instructionArg2])) {
                    cpu->VF = 1;
                } else {
                    cpu->VF = 0;
                }
            } else if (instructionArg3 == 0x6) {
                *(cpu->regptr[instructionArg1]) = *(cpu->regptr[instructionArg1]) / 2;
                if (*(cpu->regptr[instructionArg1]) & 0b00000001) {
                    cpu->VF = 1;
                } else {
                    cpu->VF = 0;
                }
            } else if (instructionArg3 == 0x7) {
                *(cpu->regptr[instructionArg1]) = *(cpu->regptr[instructionArg2]) - *(cpu->regptr[instructionArg1]);
                if (*(cpu->regptr[instructionArg3]) > *(cpu->regptr[instructionArg2])) {
                    cpu->VF = 1;
                } else {
                    cpu->VF = 0;
                }
            } else if (instructionArg3 == 0xE) {
                *(cpu->regptr[instructionArg1]) = *(cpu->regptr[instructionArg1]) * 2;
                if (*(cpu->regptr[instructionArg1]) & 0b10000000) {
                    cpu->VF = 1;
                } else {
                    cpu->VF = 0;
                }
            }
        } else if (instructionHeader == 0x9) {
            if (*(cpu->regptr[instructionArg2]) != *(cpu->regptr[instructionArg3])) {
                cpu->PC += 2;
            }
        } else if (instructionHeader == 0xA) {
            cpu->I = instructionArg1to3;
        } else if (instructionHeader == 0xB) {
            cpu->PC = instructionArg1to3 + cpu->V0;
        } else if (instructionHeader == 0xC) {
            *cpu->regptr[instructionArg1] = (rand() % 255) & instructionArg2to3;
        } else if (instructionHeader == 0xD) {
            //DRW, todo
            uint8_t bytesToRead = instructionArg3;
            uint8_t *sprite = malloc(sizeof(uint8_t) * bytesToRead);
            for (int i = 0; i < bytesToRead; i++) {
                *(sprite + i) = *(cpu->ramPtr->mem + cpu->I + i);
            }
            displaySprite(cpu, *(cpu->regptr[instructionArg1]), *(cpu->regptr[instructionArg2]), bytesToRead, sprite);
            free(sprite);
        } else if (instructionHeader == 0xE) {
            if (instructionArg3 == 0xE) {
                if ((cpu->KEY[instructionArg1])) {
                    cpu->PC += 2;
                }
            } else if (instructionArg3 == 0x1) {
                if (!(cpu->KEY[instructionArg1])) {
                    cpu->PC += 2;
                }
            }
        } else if (instructionHeader == 0xF) {
            if (instructionArg3 == 0x7) {
                *(cpu->regptr[instructionArg1]) = cpu->DT;
            } else if (instructionArg3 == 0xA) {
                //wait for key press
            } else if (instructionArg2 == 1 && instructionArg3 == 0x5) {
                cpu->DT = *(cpu->regptr[instructionArg1]);
            } else if (instructionArg3 == 0x8) {
                cpu->ST = *(cpu->regptr[instructionArg1]);
            } else if (instructionArg3 == 0xE) {
                cpu->I += *(cpu->regptr[instructionArg1]);
            } else if (instructionArg3 == 0x9) {

            } else if (instructionArg3 == 0x3) {

            } else if (instructionArg2 == 5 && instructionArg3 == 0x5) {
                for (int i = 0; i < 0xF; i++) {
                    *(cpu->ramPtr->mem + cpu->I + i) = *(cpu->regptr[i]);
                }
            } else if (instructionArg2 == 6 && instructionArg3 == 0x5) {
                for (int i = 0; i < 0xF; i++) {
                    *(cpu->regptr[i]) = *(cpu->ramPtr->mem + cpu->I + i);
                }
            }
        }
        if (incrementPC)
            cpu->PC+=2;
    }
}
void parseOpcode(CHIP8_CPU *cpu, CHIP8_DISPLAY *display, uint16_t instruction) {
    printf("%x\n", instruction);
}
