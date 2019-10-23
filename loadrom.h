#ifndef LOADROM_INCLUDE
#define LOADROM_INCLUDE
#include <stdio.h>
#include <stdint.h>
size_t getFileSize(FILE *file);
FILE *openROM(char *romFileName, uint16_t *romFileSize);
#endif
