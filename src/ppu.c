#include <stdint.h>
#include "ppu.h"

struct ppu_state ppu_init() { 
    struct ppu_state ppu = {0};
    return ppu;
}

void ppu_clock(struct ppu_state *ppu) {
    if (ppu->scanline == 261) {
    }
    else if ( (ppu->scanline >= 0) && (ppu->scanline < 240) ) {
    }
    else if (ppu->scanline == 240) {
    }
    else if ( (ppu->scanline > 240) && (ppu->scanline <= 260) ) {
    }

    if (++ppu->cycle >= 340) {
        if (++ppu->scanline >= 260)
            ppu->scanline = 0;
        ppu->cycle = 0;
    } 
}