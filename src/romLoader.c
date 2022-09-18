#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "mappers.h"

#ifdef _WIN32
#include <Windows.h>
#endif

struct mapper mappers[256] = {
    {&m000_init, &m000_ppuRead, &m000_cpuRead, &m000_ppuWrite, &m000_cpuWrite}
};

void parseRom(FILE *rom) {
    uint32_t checksum;
    fread(&checksum, sizeof(uint32_t), 1, rom);
    if (checksum != 0x1A53454E)
        printf("Rom Checksum is incorrect");

    uint8_t prgBanks;
    uint8_t chrBanks;
    uint8_t flags6;
    uint8_t flags7;

    fread(&prgBanks, sizeof(uint8_t), 1, rom);
    fread(&chrBanks, sizeof(uint8_t), 1, rom);
    fread(&flags6, sizeof(uint8_t), 1, rom);
    fread(&flags7, sizeof(uint8_t), 1, rom);
    fseek(rom, sizeof(uint64_t), SEEK_CUR);
    if (flags6 & (1<<2))
        fseek(rom, 512 * sizeof(uint8_t), SEEK_CUR);

    mapper = (flags7 & 0xF0) | ((flags6 & 0xF0) >> 4);
    uint8_t *prg = malloc(sizeof(uint8_t) * (16384 * prgBanks));
    uint8_t *chr = malloc(sizeof(uint8_t) * (8192  * chrBanks));
    fread(prg, sizeof(uint8_t), (16384 * prgBanks), rom);
    fread(chr, sizeof(uint8_t), (8192  * chrBanks), rom);
    fclose(rom);

    if (flags6 & (1 << 0))
        mirroring = Vertical;
    else
        mirroring = Horizontal;
        
    mappers[mapper].init(prg, chr, prgBanks, chrBanks);
}


#ifdef _WIN32
FILE* selectRom() {
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;

    hFind = FindFirstFile("../roms/*.nes", &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE)
        exit(1);

    char *romNames;
    char **romIndexes;

    int numRoms = 1;
    while (FindNextFileA(hFind, &FindFileData)) {
        numRoms++;
    }

    romNames = malloc(sizeof(char) * 25 * numRoms);
    memset(romNames, '\0', sizeof(char) * 25 * numRoms);
    romIndexes = malloc(sizeof(char*) * numRoms);

    int i = 0;
    hFind = FindFirstFile("../roms/*.nes", &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE)
        exit(1);

    strncpy(romNames, FindFileData.cFileName, 25);
    romIndexes[0] = &romNames[0];

    while (FindNextFileA(hFind, &FindFileData)) {
        i++;
        if (i < numRoms) {
            strncpy(&romNames[25 * i], FindFileData.cFileName, 25);
            romIndexes[i] = &romNames[(25 * i)];
        }
    }

    for (int n = 0; n < numRoms; n++) {
        printf("%s\n", romIndexes[n]);
    }

    FindClose(hFind);

    FILE *rom;
    rom = fopen("../roms/nestest.nes", "rb");
    return rom;
}
#else
FILE* selectRom() {

    printf("Literally What? \n");
    FILE *rom;
    rom = fopen("../roms/nestest.nes", "rb");
    return rom;
}
#endif