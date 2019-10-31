#include <cpu.h>
void delayTimer(CHIP8_CPU *cpu) {
    if (cpu->DT != 0) {
        cpu->DT--;
    }
}
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
    for (int spriteY = 0; spriteY < spriteSize; spriteY++) {
        for (int spriteX = 0; spriteX < 8; spriteX++) {
            uint8_t bitStatus = (*(sprite + spriteY) >> (8 - spriteX)) & 1;
            uint8_t drawX, drawY;
            if (cpu->displayPtr->frameBuf[y][x] & bitStatus) {
                cpu->VF = 1;
            } else {
                cpu->VF = 0;
            }
            if (y+spriteY > 31) {
                drawY = 0;
            } else {
                drawY = y+spriteY;
            }
            if (x+spriteX > 63) {
                drawX = 0;
            } else {
                drawX = x+spriteX;
            }
            bitStatus ? printf("drawing pixel to: (%i, %i)\n", drawX, drawY) : NULL;
            cpu->displayPtr->frameBuf[drawY][drawX] = cpu->displayPtr->frameBuf[drawY][drawX] ^ bitStatus;
        }
    }
}
#define DEBUG
void executeInstructions(CHIP8_CPU *cpu, FILE *dump) {
    for (int cycles = 0; cycles <= 9; cycles++)
    {
        static uint8_t *ram = cpu->ramPtr->mem;
        
        uint8_t incrementPC = 1;
        uint16_t opcode = (ram[cpu->PC] << 8) + ram[cpu->PC + 1];
        if (cpu->PC+1 > 4095) {
            exit(-1);
        }
        uint8_t instructionHeader = (opcode >> 12) & 0xf;
        uint8_t instructionArg1 = (opcode >> 8) & 0xf;
        uint8_t instructionArg2 = (opcode >> 4) & 0xf;
        uint8_t instructionArg3 = opcode & 0xf;
        uint16_t instructionArg1to3 = opcode & 0xfff;
        uint8_t instructionArg2to3 = opcode & 0xff;
        #ifdef DEBUG
        fprintf(dump, "**************\n");
        fprintf(dump, "Will be executing instruction: 0x%02x, PC = 0x%x\n", opcode, cpu->PC);
        fprintf(dump, "DEBUG: %x,%x,%x,%x\n", instructionHeader, instructionArg1, instructionArg2, instructionArg3);
        fprintf(dump, "**************\n");
        #endif

        if (instructionHeader == 0x0 && opcode <= 255) {
            if (instructionArg3 == 0x0) {
                clearScreen(cpu->displayPtr);
            } else if (instructionArg3 == 0xE) {
                cpu->PC = cpu->STACK[cpu->SP];
                cpu->SP--;
                incrementPC = 0;
            }
        } else if (instructionHeader == 0x1) {
            cpu->PC = instructionArg1to3;
            incrementPC = 0;
        } else if (instructionHeader == 0x2) {
            cpu->SP++;
            cpu->STACK[cpu->SP] = cpu->PC;
            cpu->PC = instructionArg1to3;
            incrementPC = 0;
        } else if (instructionHeader == 0x3) {
            if (*(cpu->regptr[instructionArg1]) == instructionArg2to3) {
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
                printf("RUNNING INSTRUCTION WITH ARG1: %i", instructionArg1);
                if ((cpu->KEY[instructionArg1])) {
                    printf("skipping instr");
                    cpu->PC += 2;
                }
            } else if (instructionArg3 == 0x1) {
                printf("RUNNING 2 INSTRUCTION WITH ARG1: %i", instructionArg1);
                if (!(cpu->KEY[instructionArg1])) {
                    printf("skipping instr 2");
                    cpu->PC += 2;
                }
            }
        } else if (instructionHeader == 0xF) {
            if (instructionArg3 == 0x7) {
                *(cpu->regptr[instructionArg1]) = cpu->DT;
            } else if (instructionArg3 == 0xA) {

            } else if (instructionArg2 == 1 && instructionArg3 == 0x5) {
                cpu->DT = *(cpu->regptr[instructionArg1]);
            } else if (instructionArg3 == 0x8) {
                cpu->ST = *(cpu->regptr[instructionArg1]);
            } else if (instructionArg3 == 0xE) {
                cpu->I += *(cpu->regptr[instructionArg1]);
            } else if (instructionArg3 == 0x9) {
                cpu->I = *(cpu->ramPtr->mem + 0x50 + instructionArg1*5);
            } else if (instructionArg3 == 0x3) {
                uint8_t regValue = *(cpu->regptr[instructionArg1]);
                uint8_t ones = regValue % 10;
                uint8_t tens = (regValue / 10) % 10;
                uint8_t hundreds = (regValue / 100) % 10;
                *(cpu->ramPtr->mem + cpu->I) = hundreds;
                *(cpu->ramPtr->mem + cpu->I + 1) = tens;
                *(cpu->ramPtr->mem + cpu->I + 2) = ones;
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
/*
**************
Will be executing instruction: 0x6a02, PC = 0x200
DEBUG: 6,a,0,2
**************
**************
Will be executing instruction: 0x6b0c, PC = 0x202
DEBUG: 6,b,0,c
**************
**************
Will be executing instruction: 0x6c3f, PC = 0x204
DEBUG: 6,c,3,f
**************
**************
Will be executing instruction: 0x6d0c, PC = 0x206
DEBUG: 6,d,0,c
**************
**************
Will be executing instruction: 0xa2ea, PC = 0x208
DEBUG: a,2,e,a
**************
**************
Will be executing instruction: 0xdab6, PC = 0x20a
DEBUG: d,a,b,6
**************
**************
Will be executing instruction: 0xdcd6, PC = 0x20c
DEBUG: d,c,d,6
**************
**************
Will be executing instruction: 0x6e00, PC = 0x20e
DEBUG: 6,e,0,0
**************
**************
Will be executing instruction: 0x22d4, PC = 0x210
DEBUG: 2,2,d,4
**************
**************
Will be executing instruction: 0xa2f2, PC = 0x2d4
DEBUG: a,2,f,2
**************
**************
Will be executing instruction: 0xfe33, PC = 0x2d6
DEBUG: f,e,3,3
**************
**************
Will be executing instruction: 0xf265, PC = 0x2d8
DEBUG: f,2,6,5
**************
**************
Will be executing instruction: 0xf129, PC = 0x2da
DEBUG: f,1,2,9
**************
**************
Will be executing instruction: 0x6414, PC = 0x2dc
DEBUG: 6,4,1,4
**************
**************
Will be executing instruction: 0x6500, PC = 0x2de
DEBUG: 6,5,0,0
**************
**************
Will be executing instruction: 0xd455, PC = 0x2e0
DEBUG: d,4,5,5
**************
**************
Will be executing instruction: 0x7415, PC = 0x2e2
DEBUG: 7,4,1,5
**************
**************
Will be executing instruction: 0xf229, PC = 0x2e4
DEBUG: f,2,2,9
**************
**************
Will be executing instruction: 0xd455, PC = 0x2e6
DEBUG: d,4,5,5
**************
**************
Will be executing instruction: 0xee, PC = 0x2e8
DEBUG: 0,0,e,e
**************
**************
*/
