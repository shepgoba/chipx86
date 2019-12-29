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
    for (uint8_t k = 0; k < 0xF; k++) {
        if ((cpu->KEY >> k) & 1) {
            //rect.x = 10*k;
            cpu->displayPtr->frameBuf[0][k] ^= 1;
            //drawDisplay(cpu->displayPtr);
        } else {
            cpu->displayPtr->frameBuf[0][k] ^= 0;
        }
    }
}

static uint8_t getSingularKeyPress(CHIP8_CPU *cpu) {
    uint8_t key = 0;
    uint8_t i = 0;
    for (;;) {
        if ((cpu->KEY >> (uint16_t)i) & 1) {
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
    }
    cpu->KEY = 0;
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
            if (!bitStatus)
                continue;
            uint8_t drawX, drawY;

            if (y + spriteY > 31) {
                drawY = spriteSize - spriteY - 1;
            } else {
                drawY = y + spriteY;
            }

            if (x + spriteX > 63) {
                drawX = spriteX;
            } else {
                drawX = x + spriteX;
            }

            if (!pixelsErased) {
                if (cpu->displayPtr->frameBuf[drawY][drawX] & bitStatus) {
                    *(cpu->VF) = 1;
                    pixelsErased = 1;
                }
            }
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
//#define DEBUG
void executeInstructions(CHIP8_CPU *cpu) {
    uint8_t *ram = cpu->ramPtr->mem;

    for (int cycles = 0; cycles <= 9; cycles++)
    {
        uint16_t opcode = (ram[cpu->PC] << 8) | ram[cpu->PC + 1];
        #ifdef DEBUG
        debugDrawKeys(cpu, cpu->displayPtr);
        #endif
        // If PC goes over ram, abort the program
        if (cpu->PC+1 > 4095) {
            printf("An error occurred.\n");
            exit(-1);
        }

        uint8_t instrHeader = (opcode >> 12) & 0xf;
        uint8_t num1 = (opcode >> 8) & 0xf;
        uint8_t num2 = (opcode >> 4) & 0xf;
        uint8_t num3 = opcode & 0xf;
        uint16_t num1to3 = opcode & 0xfff;
        uint8_t num2to3 = opcode & 0xff;

        #ifdef DEBUG
        fprintf(dump, "**************\n");
        fprintf(dump, "Will be executing instruction: 0x%02x, PC = 0x%x\n", opcode, cpu->PC);
        fprintf(dump, "DEBUG: %x,%x,%x,%x\n", instrHeader, num1, num2, num3);
        fprintf(dump, "**************\n");
        #endif

        #ifdef DEBUG
        fprintf(dump2, "0x%02x, PC = 0x%x\n", opcode, cpu->PC);
        fprintf(dump2, "I: %x", cpu->I);
        fprintf(dump2, "\n");
        fprintf(dump2, "\n");
        #endif
        printf("it is: %x", instrHeader);
        switch (instrHeader) {
            case 0x0: 
                if (num3 == 0x0) {
                    clearScreen(cpu->displayPtr);
                } else if (num3 == 0xE) {
                    cpu->PC = cpu->STACK[cpu->SP];
                    cpu->SP--;
                    continue;
                }
            break;
            case 0x1: 
                cpu->PC = num1to3;
                continue;
            break;
            case 0x2: 
                cpu->SP++;
                cpu->STACK[cpu->SP] = cpu->PC + 2;
                cpu->PC = num1to3;
                continue;
            break;
            case 0x3:
                if (cpu->reg[num1] == num2to3) {
                    cpu->PC += 4;
                    continue;
                }
            break;
            case 0x4:
                if (cpu->reg[num1] != num2to3) {
                    cpu->PC += 4;
                    continue;
                }  
            break;
            case 0x5: 
                if (cpu->reg[num1] == cpu->reg[num2]) {
                    cpu->PC += 4;
                    continue;
                } 
            break;
            case 0x6: 
                cpu->reg[num1] = num2to3; 
            break;
            case 0x7: 
                cpu->reg[num1] += num2to3;
            break;
            case 0x8: 
                if (num3 == 0x0) {
                    cpu->reg[num1] = cpu->reg[num2];
                } else if (num3 == 0x1) {
                    cpu->reg[num1] = cpu->reg[num1] | cpu->reg[num2];
                } else if (num3 == 0x2) {
                    cpu->reg[num1] = cpu->reg[num1] & cpu->reg[num2];
                } else if (num3 == 0x3) {
                    cpu->reg[num1] = cpu->reg[num1] ^ cpu->reg[num2];
                } else if (num3 == 0x4) {
                    //The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0. Only the lowest 8 bits of the result are kept, and stored in Vx.
                    cpu->reg[num1] += cpu->reg[num2];
                    if (cpu->reg[num1] + cpu->reg[num2] != (cpu->reg[num1] + cpu->reg[num3] & 0xFF)) {
                        *(cpu->VF) = 1;
                    } else {
                        *(cpu->VF) = 0;
                    }
                } else if (num3 == 0x5) {
                    if (cpu->reg[num1] > cpu->reg[num2]) {
                        *(cpu->VF) = 1;
                    } else {
                        *(cpu->VF) = 0;
                    }
                    cpu->reg[num1] -= cpu->reg[num2];
                } else if (num3 == 0x6) {
                    if (cpu->reg[num1] & 0b00000001) {
                        *(cpu->VF) = 1;
                    } else {
                        *(cpu->VF) = 0;
                    }
                    cpu->reg[num1] = cpu->reg[num1] / 2;
                } else if (num3 == 0x7) {
                    if (cpu->reg[num3] > cpu->reg[num2]) {
                        *(cpu->VF) = 1;
                    } else {
                        *(cpu->VF) = 0;
                    }
                    cpu->reg[num1] = cpu->reg[num2] - cpu->reg[num1];
                } else if (num3 == 0xE) {
                    if (cpu->reg[num1] & 0b10000000) {
                        *(cpu->VF) = 1;
                    } else {
                        *(cpu->VF) = 0;
                    }
                    cpu->reg[num1] = cpu->reg[num1] * 2;
                }
            break;
            case 0x9:
                if (cpu->reg[num1] != cpu->reg[num2]) {
                    cpu->PC += 4;
                    continue;
                }
            break;
            case 0xA: 
                cpu->I = num1to3;
            break;
            case 0xB: 
                cpu->PC = num1to3 + cpu->reg[0];
                continue;
            break;
            case 0xC: 
                cpu->reg[num1] = (rand() % 255) & num2to3;
            break;
            case 0xD:
                displaySprite(cpu, cpu->reg[num1], cpu->reg[num2], num3, cpu->ramPtr->mem + cpu->I);
            break;
            case 0xE: 
                if (num3 == 0xE) {
                    if (((cpu->KEY >> cpu->reg[num1]) & 1)) {
                        cpu->PC += 4;
                        continue;
                    }
                } else if (num3 == 0x1) {
                    if (!((cpu->KEY >> cpu->reg[num1]) & 1)) {
                        cpu->PC += 4;
                        continue;
                    }
                }
            break;
            case 0xF:
                if (num3 == 0x7) {
                    cpu->reg[num1] = cpu->DT;
                } else if (num3 == 0xA) {
                    cpu->reg[num1] = getSingularKeyPress(cpu);
                } else if (num2 == 0x1 && num3 == 0x5) {
                    cpu->DT = cpu->reg[num1];
                } else if (num3 == 0x8) {
                    cpu->ST = cpu->reg[num1];
                } else if (num3 == 0xE) {
                    cpu->I += cpu->reg[num1];
                } else if (num3 == 0x9) {
                    cpu->I = 0x50 + cpu->reg[num1] * 5;
                    //printf("data: %x\n", cpu->ramPtr->mem[0x50 + num1*5]);
                } else if (num3 == 0x3) {
                    uint8_t regValue = cpu->reg[num1];
                    uint8_t ones = regValue % 10;
                    uint8_t tens = (regValue / 10) % 10;
                    uint8_t hundreds = (regValue / 100) % 10;
                    cpu->ramPtr->mem[cpu->I] = hundreds;
                    cpu->ramPtr->mem[cpu->I + 1] = tens;
                    cpu->ramPtr->mem[cpu->I + 2] = ones;
                } else if (num2 == 5 && num3 == 0x5) {
                    for (int i = 0; i <= num1; i++) {
                        cpu->ramPtr->mem[cpu->I + i] = cpu->reg[i];
                    }
                } else if (num2 == 6 && num3 == 0x5) {
                    for (int i = 0; i <= num1; i++) {
                        cpu->reg[i] = cpu->ramPtr->mem[cpu->I + i];
                    }
                }
            break;
        }
        cpu->PC += 2;
    }
}