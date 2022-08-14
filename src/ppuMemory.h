#ifndef PPUMEMORY_H
#define PPUMEMORY_H

#include <stdint.h>

union PPUControlRegister {
    struct {
        uint8_t nametable_x : 1;
        uint8_t nametable_y : 1;
        uint8_t incrementMode : 1;
        uint8_t spriteTable : 1;
        uint8_t backgroundTable : 1;
        uint8_t spriteHeight : 1;
        uint8_t PPUMasterSlave : 1;
        uint8_t generateNMI : 1;
    };
    uint8_t reg;
} ppuControl;

union PPUMaskRegister {
    struct {
        uint8_t greyscale : 1;
        uint8_t backgroundLeftColumn : 1;
        uint8_t spritesLeftColumn : 1;
        uint8_t showBackground : 1;
        uint8_t showSprites : 1;
        uint8_t emphasiseRed : 1;
        uint8_t emphasiseGreen : 1;
        uint8_t emphasiseBlue : 1;
    };
    uint8_t reg;
} ppuMask;

union PPUStatusRegister {
    struct {
        uint8_t unused : 5;
        uint8_t spriteOverflow : 1;
        uint8_t spriteZeroHit : 1;
        uint8_t vBlank : 1;
    };
    uint8_t reg;
} ppuStatus;

union loopyRegister {
    struct {
        uint16_t coarseX : 5;
        uint16_t coarseY : 5;
        uint16_t nametable_x : 1;
        uint16_t nametable_y : 1;
        uint16_t fine_y : 3;
        uint16_t unused : 1;
    };
    uint16_t reg;
};

union loopyRegister loopyT;
union loopyRegister loopyV;
uint8_t ppuData;
int ppuFineX;

void ppu_write(uint16_t address, uint8_t data);
uint8_t ppu_read(uint16_t address);

#endif