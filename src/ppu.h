#ifndef PPU_H
#define PPU_H

#include <stdint.h>

struct ppu_state {
    int scanline, cycle;
    uint8_t pixels[256 * 240];
};

struct ppu_state ppu_init();
int ppu_clock(struct ppu_state *ppu);

#endif