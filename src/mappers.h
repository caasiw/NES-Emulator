#ifndef MAPPERS_H
#define MAPPERS_H

#include <stdint.h>

uint8_t mapper;

struct mapper {
    void (*init)(uint8_t *prg, uint8_t *chr);
    uint8_t (*read)(uint16_t address);
    void (*write)(uint16_t address, uint8_t data);
};

void mapper000_init(uint8_t *prg, uint8_t *chr);
void mapper000_cpu_write(uint16_t address, uint8_t data);
uint8_t mapper000_cpu_read(uint16_t address);
void mapper000_ppu_write(uint16_t address, uint8_t data);
uint8_t mapper000_ppu_read(uint16_t address);

struct mapper mappers[256] = {
    {&mapper000_init, mapper000_read, mapper000_write}
};

#endif