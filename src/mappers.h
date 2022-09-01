#ifndef MAPPERS_H
#define MAPPERS_H

#include <stdint.h>

uint8_t mapper;

enum mirroring {
    Horizontal,
    Vertical,
    OneScreen,
    FourScreen
} mirroring;

struct mapper {
    void    (*init)(uint8_t *prg, uint8_t *chr, int prgBanks, int chrBanks);
    uint8_t (*ppuRead)(uint16_t);
    uint8_t (*cpuRead)(uint16_t);
    void    (*ppuWrite)(uint16_t, uint8_t);
    void    (*cpuWrite)(uint16_t, uint8_t);
};

void m000_init(uint8_t *prg, uint8_t *chr, int prgBanks, int chrBanks);
void m000_cpuWrite(uint16_t address, uint8_t data);
uint8_t m000_cpuRead(uint16_t address);
void m000_ppuWrite(uint16_t address, uint8_t data);
uint8_t m000_ppuRead(uint16_t address);

struct mapper mappers[256];

#endif