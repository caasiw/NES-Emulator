#ifndef PPU_H
#define PPU_H

#include <stdint.h>

struct ppu_state {
    int scanline, cycle;
    uint8_t *pixels;
};

struct ppu_state ppu_init();
void ppu_clock(struct ppu_state *ppu);

#endif