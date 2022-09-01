#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "mappers.h"

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

FILE* selectRom() {
    FILE *rom;
    rom = fopen("../roms/nestest.nes", "rb");
    return rom;
}