#include <cpu.h>
void delayTimer(CHIP8_CPU *cpu) {
    if (cpu->DT != 0) {
        cpu->DT--;
    }
}
static void dumpRamToFile(CHIP8_CPU *cpu) {
    FILE *file = fopen("ramdump.bin", "wb+");
    fwrite(cpu->ramPtr->mem, 1, 4096, file);
    fclose(file);
}
void debugDrawKeys(CHIP8_CPU *cpu, CHIP8_DISPLAY *display) {
    static SDL_Rect rect;
    rect.w = 10;
    rect.h = 10;
    rect.y = 10;
    for (int k = 0; k < 0xF; k++) {
        if (cpu->KEY[k]) {
            //rect.x = 10*k;
            cpu->displayPtr->frameBuf[0][k] ^= 1;
            //drawDisplay(cpu->displayPtr);
        } else {
            cpu->displayPtr->frameBuf[0][k] ^= 0;
        }
    }
}

static uint8_t getSingularKeyPress(CHIP8_CPU *cpu) {
    int key = 0;
    int i = 0;
    for (;;) {
        if (cpu->KEY[i]) {
            return i;
        }
        i++;
        if (i >= 0xF) {
            i = 0;
        }
    }
    return 0;
}

void initCPU(CHIP8_CPU *cpu, CHIP8_RAM *ram, CHIP8_DISPLAY *display) {
    for (int i = 0; i < 0xF; i++) {
        cpu->reg[i] = 0;
        cpu->KEY[i] = 0;
    }
    for (int i = 0; i < 24; i++) {
        cpu->STACK[i] = 0;
    }
    cpu->DT = 0;
    cpu->ST = 0;
    cpu->VF = &cpu->reg[15];
    cpu->SP = 0;
    cpu->I = 0;
    cpu->PC = 0x200;
    cpu->ramPtr = ram;
    cpu->displayPtr = display;
}

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

void displaySprite(CHIP8_CPU *cpu, uint8_t x, uint8_t y, uint8_t spriteSize, uint8_t *sprite) {
    uint8_t pixelsErased = 0;
    for (int spriteY = 0; spriteY < spriteSize; spriteY++) {
        for (int spriteX = 0; spriteX < 8; spriteX++) {
            uint8_t bitStatus = (*(sprite + spriteY) >> (7 - spriteX)) & 1;
            //printf("(%i (%i), %i (%i)): %i\n", x, spriteX, y, spriteY, bitStatus);
            if (!bitStatus)
                continue;
            uint8_t drawX, drawY;
            if (y+spriteY > 31) {
                drawY = spriteY;
            } else {
                drawY = y+spriteY;
            }
            if (x+spriteX > 63) {
                drawX = spriteX;
            } else {
                drawX = x+spriteX;
            }
            //printf("draw: (%i, %i)\n", drawX, drawY);
            if (!pixelsErased) {
                if (cpu->displayPtr->frameBuf[drawY][drawX] & bitStatus) {
                    *(cpu->VF) = 1;
                    pixelsErased = 1;
                }
            }
            //if (bitStatus) printf("drawing pixel to: (%i, %i)\n", drawX, drawY);
            cpu->displayPtr->frameBuf[drawY][drawX] ^= bitStatus;
        }
    }
    if (!pixelsErased)
        *(cpu->VF) = 0;
    cpu->displayPtr->frameCount++;
    SDL_Renderer *renderer = cpu->displayPtr->renderer;
    SDL_RenderClear(renderer);
    drawDisplay(cpu->displayPtr);
    SDL_RenderPresent(renderer);
    //printf("drawing frame: %i\n", cpu->displayPtr->frameCount);
}
#define DEBUG
void executeInstructions(CHIP8_CPU *cpu, FILE *dump, FILE *dump2) {
    uint8_t *ram = cpu->ramPtr->mem;

    for (int cycles = 0; cycles <= 9; cycles++)
    {
        uint16_t opcode = (ram[cpu->PC] << 8) | ram[cpu->PC + 1];
        #ifdef DEBUG
        debugDrawKeys(cpu, cpu->displayPtr);
        #endif
        // If PC goes over ram, abort the program
        if (cpu->PC+1 > 4095) {
            exit(-1);
        }

        uint8_t instructionHeader = (opcode >> 12) & 0xf;
        uint8_t instructionArg1 = (opcode >> 8) & 0xf;
        uint8_t instructionArg2 = (opcode >> 4) & 0xf;
        uint8_t instructionArg3 = opcode & 0xf;
        uint16_t instructionArg1to3 = opcode & 0xfff;
        uint8_t instructionArg2to3 = opcode & 0xff;
        /*#ifdef DEBUG
        fprintf(dump, "**************\n");
        fprintf(dump, "Will be executing instruction: 0x%02x, PC = 0x%x\n", opcode, cpu->PC);
        fprintf(dump, "DEBUG: %x,%x,%x,%x\n", instructionHeader, instructionArg1, instructionArg2, instructionArg3);
        fprintf(dump, "**************\n");
        #endif*/

        if (instructionHeader == 0x0 && opcode <= 255) {
            if (instructionArg3 == 0x0) {
                clearScreen(cpu->displayPtr);
            } else if (instructionArg3 == 0xE) {
                cpu->PC = cpu->STACK[cpu->SP];
                cpu->SP--;
                continue;
            }
        } else if (instructionHeader == 0x1) {
            cpu->PC = instructionArg1to3;
            continue;
        } else if (instructionHeader == 0x2) {
            cpu->SP++;
            cpu->STACK[cpu->SP] = cpu->PC;
            cpu->PC = instructionArg1to3;
            continue;
        } else if (instructionHeader == 0x3) {
            if (cpu->reg[instructionArg1] == instructionArg2to3) {
                cpu->PC += 4;
                continue;
            }
        } else if (instructionHeader == 0x4) {
            if (cpu->reg[instructionArg1] != instructionArg2to3) {
                cpu->PC += 4;
                continue;
            }
        } else if (instructionHeader == 0x5) {
            if (cpu->reg[instructionArg1] == cpu->reg[instructionArg2]) {
                cpu->PC += 4;
                continue;
            }
        } else if (instructionHeader == 0x6) {
            cpu->reg[instructionArg1] = instructionArg2to3;
        } else if (instructionHeader == 0x7) {
            cpu->reg[instructionArg1] += instructionArg2to3;
        } else if (instructionHeader == 0x8) {
            if (instructionArg3 == 0x0) {
                cpu->reg[instructionArg1] = cpu->reg[instructionArg2];
            } else if (instructionArg3 == 0x1) {
                cpu->reg[instructionArg1] = cpu->reg[instructionArg1] | cpu->reg[instructionArg2];
            } else if (instructionArg3 == 0x2) {
                cpu->reg[instructionArg1] = cpu->reg[instructionArg1] & cpu->reg[instructionArg2];
            } else if (instructionArg3 == 0x3) {
                cpu->reg[instructionArg1] = cpu->reg[instructionArg1] ^ cpu->reg[instructionArg2];
            } else if (instructionArg3 == 0x4) {
                //The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0. Only the lowest 8 bits of the result are kept, and stored in Vx.
                cpu->reg[instructionArg1] += cpu->reg[instructionArg2];
                if (cpu->reg[instructionArg1] + cpu->reg[instructionArg2] != (cpu->reg[instructionArg1] + cpu->reg[instructionArg3] & 0xFF)) {
                    *(cpu->VF) = 1;
                } else {
                    *(cpu->VF) = 0;
                }
            } else if (instructionArg3 == 0x5) {
                if (cpu->reg[instructionArg1] > cpu->reg[instructionArg2]) {
                    *(cpu->VF) = 1;
                } else {
                    *(cpu->VF) = 0;
                }
                cpu->reg[instructionArg1] -= cpu->reg[instructionArg2];
            } else if (instructionArg3 == 0x6) {
                if (cpu->reg[instructionArg1] & 0b00000001) {
                    *(cpu->VF) = 1;
                } else {
                    *(cpu->VF) = 0;
                }
                 cpu->reg[instructionArg1] = cpu->reg[instructionArg1] / 2;
            } else if (instructionArg3 == 0x7) {
                if (cpu->reg[instructionArg3] > cpu->reg[instructionArg2]) {
                    *(cpu->VF) = 1;
                } else {
                    *(cpu->VF) = 0;
                }
                cpu->reg[instructionArg1] = cpu->reg[instructionArg2] - cpu->reg[instructionArg1];
            } else if (instructionArg3 == 0xE) {
                if (cpu->reg[instructionArg1] & 0b10000000) {
                    *(cpu->VF) = 1;
                } else {
                    *(cpu->VF) = 0;
                }
                cpu->reg[instructionArg1] = cpu->reg[instructionArg1] * 2;
            }
        } else if (instructionHeader == 0x9) {
            if (cpu->reg[instructionArg1] != cpu->reg[instructionArg2]) {
                cpu->PC += 4;
                continue;
            }
        } else if (instructionHeader == 0xA) {
            cpu->I = instructionArg1to3;
        } else if (instructionHeader == 0xB) {
            cpu->PC = instructionArg1to3 + cpu->reg[0];
            continue;
        } else if (instructionHeader == 0xC) {
            cpu->reg[instructionArg1] = (rand() % 255) & instructionArg2to3;
        } else if (instructionHeader == 0xD) {
            //DRW, todo
            fprintf(dump, "NEW DRAW\n");
            uint8_t bytesToRead = instructionArg3;
            uint8_t *sprite = calloc(bytesToRead, sizeof(uint8_t));
            for (int i = 0; i < bytesToRead; i++) {
                *(sprite + i) = cpu->ramPtr->mem[cpu->I + i];
                fprintf(dump, BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(*(sprite + i)));
                fprintf(dump, "\n");
            }
            fprintf(dump,"\n");
            displaySprite(cpu, cpu->reg[instructionArg1], cpu->reg[instructionArg2], bytesToRead, sprite);
            free(sprite);
        } else if (instructionHeader == 0xE) {
            if (instructionArg3 == 0xE) {
                printf("RUNNING INSTRUCTION WITH ARG1: %i", instructionArg1);
                if ((cpu->KEY[cpu->reg[instructionArg1]])) {
                    printf("skipping instr");
                    cpu->PC += 4;
                    continue;
                }
            } else if (instructionArg3 == 0x1) {
                printf("RUNNING 2 INSTRUCTION WITH ARG1: %i", instructionArg1);
                if (!(cpu->KEY[cpu->reg[instructionArg1]])) {
                    printf("skipping instr 2");
                    cpu->PC += 4;
                    continue;
                }
            }
        } else if (instructionHeader == 0xF) {
            if (instructionArg3 == 0x7) {
                cpu->reg[instructionArg1] = cpu->DT;
            } else if (instructionArg3 == 0xA) {
                cpu->reg[instructionArg1] = getSingularKeyPress(cpu);
            } else if (instructionArg2 == 1 && instructionArg3 == 0x5) {
                cpu->DT = cpu->reg[instructionArg1];
            } else if (instructionArg3 == 0x8) {
                cpu->ST = cpu->reg[instructionArg1];
            } else if (instructionArg3 == 0xE) {
                cpu->I += cpu->reg[instructionArg1];
            } else if (instructionArg3 == 0x9) {
                cpu->I = 0x50 + cpu->reg[instructionArg1] * 5;
                //printf("data: %x\n", cpu->ramPtr->mem[0x50 + instructionArg1*5]);
            } else if (instructionArg3 == 0x3) {
                uint8_t regValue = cpu->reg[instructionArg1];
                uint8_t ones = regValue % 10;
                uint8_t tens = (regValue / 10) % 10;
                uint8_t hundreds = (regValue / 100) % 10;
                cpu->ramPtr->mem[cpu->I] = hundreds;
                cpu->ramPtr->mem[cpu->I + 1] = tens;
                cpu->ramPtr->mem[cpu->I + 2] = ones;
                //printf("num: %i, %i, %i, %i", cpu->reg[instructionArg1], hundreds, tens, ones);
            } else if (instructionArg2 == 5 && instructionArg3 == 0x5) {
                for (int i = 0; i < 0xF; i++) {
                    cpu->ramPtr->mem[cpu->I + i] = cpu->reg[i];
                }
            } else if (instructionArg2 == 6 && instructionArg3 == 0x5) {
                for (int i = 0; i < 0xF; i++) {
                    cpu->reg[i] = cpu->ramPtr->mem[cpu->I + i];
                }
            }
        }
        #ifdef DEBUG
        fprintf(dump2, "0x%02x, PC = 0x%x\n", opcode, cpu->PC);
        for (int i = 0; i < 16; i++) {
            fprintf(dump2, "V%x: %i, ", i, cpu->reg[i]);
        }
        fprintf(dump2, "I: %x", cpu->I);
        fprintf(dump2, "\n");
        fprintf(dump2, "\n");
        #endif
        cpu->PC += 2;
    }
}