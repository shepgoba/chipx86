void CLS(CHIP8_CPU *cpu) {
    clearScreen(cpu->displayPtr);
}

void RET(CHIP8_CPU *cpu) {
    cpu->PC = cpu->STACK[cpu->SP];
    cpu->SP--;
}

void JP(CHIP8_CPU *cpu) {
    uint16_t opcode = (cpu->ramPtr->mem[cpu->PC] << 8) + cpu->ramPtr->mem[cpu->PC];
    cpu->PC = instructionArg1to3;
}
void CALL(CHIP8_CPU *cpu) {
    uint16_t opcode = (cpu->ramPtr->mem[cpu->PC] << 8) + cpu->ramPtr->mem[cpu->PC];
    uint16_t instructionArg1to3 = opcode & 0xfff;
    cpu->SP++;
    cpu->STACK[cpu->SP] = cpu->PC;
    cpu->PC = instructionArg1to3;
}
void SEBYTE(CHIP8_CPU *cpu) {
    uint16_t opcode = (cpu->ramPtr->mem[cpu->PC] << 8) + cpu->ramPtr->mem[cpu->PC];
    uint16_t instructionArg1to3 = opcode & 0xfff;
    if (cpu->reg[instructionArg1] == instructionArg2to3) {
        cpu->PC += 2;
    }
}
         else if (instructionHeader == 0x3) {
            if (cpu->reg[instructionArg1] == instructionArg2to3) {
                cpu->PC += 2;
            }
        } else if (instructionHeader == 0x4) {
            if (cpu->reg[instructionArg1] != instructionArg2to3) {
                cpu->PC += 2;
            }
        } else if (instructionHeader == 0x5) {
            if (cpu->reg[instructionArg1] == cpu->reg[instructionArg2]) {
                cpu->PC += 2;
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
                cpu->reg[instructionArg1] -= cpu->reg[instructionArg2];
                if (cpu->reg[instructionArg1] > cpu->reg[instructionArg2]) {
                    *(cpu->VF) = 1;
                } else {
                    *(cpu->VF) = 0;
                }
            } else if (instructionArg3 == 0x6) {
                cpu->reg[instructionArg1] = cpu->reg[instructionArg1] / 2;
                if (cpu->reg[instructionArg1] & 0b00000001) {
                    *(cpu->VF) = 1;
                } else {
                    *(cpu->VF) = 0;
                }
            } else if (instructionArg3 == 0x7) {
                cpu->reg[instructionArg1] = cpu->reg[instructionArg2] - cpu->reg[instructionArg1];
                if (cpu->reg[instructionArg3] > cpu->reg[instructionArg2]) {
                    *(cpu->VF) = 1;
                } else {
                    *(cpu->VF) = 0;
                }
            } else if (instructionArg3 == 0xE) {
                cpu->reg[instructionArg1] = cpu->reg[instructionArg1] * 2;
                if (cpu->reg[instructionArg1] & 0b10000000) {
                    *(cpu->VF) = 1;
                } else {
                    *(cpu->VF) = 0;
                }
            }
        } else if (instructionHeader == 0x9) {
            if (cpu->reg[instructionArg2] != cpu->reg[instructionArg3]) {
                cpu->PC += 2;
            }
        } else if (instructionHeader == 0xA) {
            cpu->I = instructionArg1to3;
        } else if (instructionHeader == 0xB) {
            cpu->PC = instructionArg1to3 + cpu->reg[0];
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
                cpu->reg[instructionArg1] = cpu->DT;
            } else if (instructionArg3 == 0xA) {

            } else if (instructionArg2 == 1 && instructionArg3 == 0x5) {
                cpu->DT = cpu->reg[instructionArg1];
            } else if (instructionArg3 == 0x8) {
                cpu->ST = cpu->reg[instructionArg1];
            } else if (instructionArg3 == 0xE) {
                cpu->I += cpu->reg[instructionArg1];
            } else if (instructionArg3 == 0x9) {
                cpu->I = cpu->ramPtr->mem[0x50 + instructionArg1*5];
                printf("data: %x\n", cpu->ramPtr->mem[0x50 + instructionArg1*5]);
            } else if (instructionArg3 == 0x3) {
                uint8_t regValue = cpu->reg[instructionArg1];
                uint8_t ones = regValue % 10;
                uint8_t tens = (regValue / 10) % 10;
                uint8_t hundreds = (regValue / 100) % 10;
                cpu->ramPtr->mem[cpu->I] = hundreds;
                cpu->ramPtr->mem[cpu->I + 1] = tens;
                cpu->ramPtr->mem[cpu->I + 2] = ones;
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