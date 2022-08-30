#include <stdint.h>
#include "ppu.h"
#include "ppuMemory.h"

uint16_t shiftHI, shiftLO, shiftPHI, shiftPLO;
uint8_t nextID, nextAtt, nextLO, nextHI;

struct ppu_state ppu_init() { 
    struct ppu_state ppu = {0};
    return ppu;
}

void scrollX() {
    if (ppuMask.showBackground || ppuMask.showSprites) {
        if (++loopyV.coarseX == 0)
            loopyV.nametable_x = ~loopyV.nametable_x;
    }
}

void scrollY() {
    if (ppuMask.showBackground || ppuMask.showSprites) {
        if (++loopyV.fine_y == 0) {
            if (++loopyV.coarseY >= 30) {
                loopyV.coarseY = 0;
                loopyV.nametable_y = ~loopyV.nametable_y;
            }
        }
    }
}

void transferX() {
    if (ppuMask.showBackground || ppuMask.showSprites) {
        loopyV.nametable_x = loopyT.nametable_x;
        loopyV.coarseX = loopyT.coarseX;
    }
}

void transferY() {
    if (ppuMask.showBackground || ppuMask.showSprites) {
        loopyV.nametable_y = loopyT.nametable_y;
        loopyV.coarseY = loopyT.coarseY;
        loopyV.fine_y = loopyT.fine_y;
    }
}

void loadRegisters() {
    shiftPLO = (shiftPLO & 0xFF00) | nextLO;
    shiftPHI = (shiftPHI & 0xFF00) | nextHI;
    shiftLO = (shiftLO & 0xFF00) | ( (nextAtt & 0x01) ? 0xFF : 0x00);
    shiftHI = (shiftHI & 0xFF00) | ( (nextAtt & 0x02) ? 0xFF : 0x00);
}

void shiftRegisters() {
    if (ppuMask.showBackground) {
        shiftPLO <<= 1;
        shiftPHI <<= 1;
        shiftHI <<= 1;
        shiftLO <<= 1;
    }
}

void ppu_clock(struct ppu_state *ppu) {

    // Pre Render
    if (ppu->scanline == 261) {
        if(ppu->cycle = 1) {
            ppuStatus.vBlank = 0;
            ppuStatus.spriteZeroHit = 0;
            ppuStatus.spriteOverflow = 0;
        }

        if( (ppu->cycle >= 280) && (ppu->cycle < 305) )
            transferY();
    }

    // Visible and Pre Render
    if ( ((ppu->scanline >= 0)&&(ppu->scanline < 240))||(ppu->scanline==261)) {
        if ( ((ppu->cycle >= 1) && (ppu->cycle <= 256)) 
          || ((ppu->cycle >= 321) && (ppu->cycle < 337))) {
            shiftRegisters();
            switch (ppu->cycle % 8) {
                case 1 :
                    loadRegisters();
                    nextID = ppu_read(0x2000 | (loopyV.reg & 0x0FFF));
                    break;
                case 3 :
                    nextAtt = ppu_read(0x23C0 | (loopyV.nametable_y << 11)
                                              | (loopyV.nametable_x << 10)
                                              | ((loopyV.coarseY >> 2) << 3)
                                              | (loopyV.coarseX >> 2));

                    if (loopyV.coarseY & 0x02)
                        nextAtt >>= 4;
                    if (loopyV.coarseX & 0x02)
                        nextAtt >>= 2;
                    nextAtt &= 0x03;
                    break;
                case 5 :
                    nextLO = ppu_read((ppuControl.backgroundTable << 12)
                                     + (((uint16_t)(nextID)) << 4)
                                     + loopyV.fine_y + 0);
                    break;
                case 7 :
                    nextHI = ppu_read((ppuControl.backgroundTable << 12)
                                     + (((uint16_t)(nextID)) << 4)
                                     + loopyV.fine_y + 8);
                    break;
                case 0 :
                    scrollX();
                    break;
            }
        }
        else if (ppu->cycle == 257) {
            scrollY();
            transferX();
        }
        else if ((ppu->cycle == 337) || (ppu->cycle == 339))
            nextID = ppu_read(0x2000 | loopyV.reg & 0x0FFF);
    }

    // Post Render
    else if (ppu->scanline == 240) {
    }

    // VBlank
    else if ( (ppu->scanline > 240) && (ppu->scanline <= 260) ) {
        if ((ppu->scanline == 241) && (ppu->cycle == 1)) {
            ppuStatus.vBlank = 1;
            if (ppuControl.generateNMI) {
                
            }
        }

    }

    if (++ppu->cycle >= 340) {
        if (++ppu->scanline >= 262)
            ppu->scanline = 0;
        ppu->cycle = 0;
    } 
}