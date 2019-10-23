#include "loadrom.h"
size_t getFileSize(FILE *file) {
    size_t orig = ftell(file);
    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file, 0, orig);
    return fileSize;
}

FILE *openROM(char *romFileName, uint16_t *romFileSize) {
    FILE *romFile = fopen(romFileName, "rb");
    if (!romFile) {
        printf("File \"%s\" not found!\n", romFileName);
        return NULL;
    }
    uint16_t maxRomSize = 4096;
    uint16_t romSize = getFileSize(romFile);
    if (romFileSize != NULL) {
        *(romFileSize) = romSize;
    }
    if (romSize > maxRomSize) {
        printf("ROM file %s is too big! (4KiB max)\n", romFileName);
        return NULL;
    }
    return romFile;
}
