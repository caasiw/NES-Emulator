#include <stdint.h>
#include "mappers.h"

uint8_t nametable1[1024], nametable2[1024];
uint8_t palette[32];

void ppu_write(uint16_t address, uint8_t data) {
    if ( (address < 0x2000) && (address >= 0x0000) ) {
        mappers[mapper].ppuWrite(address, data);
    }
    else if ( (address < 0x3F00) && (address >= 0x2000) ) {
        switch (mirroring) {
            case Horizontal :
                if (address < 0x2800)
                    nametable1[(address % 0x0400)] = data;
                else
                    nametable2[(address % 0x0400)] = data;
                break;
            case Vertical :
                if ((address / 0x0400) % 2)
                    nametable2[(address % 0x0400)] = data;
                else
                    nametable1[(address % 0x0400)] = data;
                break;
            case OneScreen :
                break;
            case FourScreen :
                break;
        }
    }
    else if ( (address <= 0x3FFF) && (address >= 0x3F00) ) {
        int temp = address - 0x3F00;
        if ((temp % 4) == 0) 
            palette[0] = data;
        else
            palette[(temp / 4) + (temp % 4)] = data;
    }
}


uint8_t ppu_read(uint16_t address) {
    if ( (address < 0x2000) && (address >= 0x0000) ) {
        return mappers[mapper].ppuRead(address);
    }
    else if ( (address < 0x3F00) && (address >= 0x2000) ) {
        switch (mirroring) {
            case Horizontal :
                if (address < 0x2800)
                    return nametable1[(address % 0x0400)];
                else
                    return nametable2[(address % 0x0400)];
                break;
            case Vertical :
                if ((address / 0x0400) % 2)
                    return nametable2[(address % 0x0400)];
                else
                    return nametable1[(address % 0x0400)];
                break;
            case OneScreen :
                break;
            case FourScreen :
                break;
        }
    }
    else if ( (address <= 0x3FFF) && (address >= 0x3F00) ) {
        int temp = address - 0x3F00;
        if ((temp % 4) == 0) 
            return palette[0];
        else
            return palette[(temp / 4) + (temp % 4)];
    }
    return 0;
}