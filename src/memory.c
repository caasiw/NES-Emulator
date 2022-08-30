#include <stdint.h>
#include "memory.h"
#include "ppuMemory.h"
#include "mappers.h"

uint8_t ram[0x0800];
int ppuLatch = 0;

void cpu_write(uint16_t address, uint8_t data) {
    if ( (address < 0x2000) && (address >= 0x0000) ) {
        ram[(address % 0x0800)] = data;
        return;
    }
    else if ( (address < 0x4000) && (address >= 0x2000) ) {
        switch (address % 8) {
            case 0 :
                ppuControl.reg = data;
                loopyT.nametable_x = ppuControl.nametable_x;
                loopyT.nametable_y = ppuControl.nametable_y;
                break;
            case 1 :
                ppuMask.reg = data;
                break;
            case 2 :
                break;
            case 3 :
                break;
            case 4 :
                break;
            case 5 :
                if (ppuLatch == 0) {
                    ppuFineX = data & 0x07;
                    loopyT.reg = data >> 3;
                    ppuLatch++;
                }
                else {
                    loopyT.fine_y = data & 0x07;
                    loopyT.coarseY = data >> 3;
                    ppuLatch = 0;
                }
                break;
            case 6 :
                if (ppuLatch == 0) {
                    loopyT.reg = ((data & 0x3F) << 8) | (loopyT.reg & 0x00FF);
                    ppuLatch++;
                }
                else {
                    loopyT.reg = (loopyT.reg & 0xFF00) | data;
                    loopyV.reg = loopyT.reg;
                    ppuLatch = 0;
                }
                break;
            case 7 :
                ppu_write(loopyV.reg, data);
                loopyV.reg += ( (ppuControl.incrementMode) ? 32 : 1);
                break;
        }
    }
    else if ( (address < 0x4020) && (address >= 0x4000) ) {
        // APU and IO Registers
    }
    else if ( (address <= 0xFFFF) && (address >= 0x4020) ) {
        mappers[mapper].cpuWrite(address, data);
    }
}

uint8_t cpu_read(uint16_t address) {
    if ( (address < 0x2000) && (address >= 0x0000) ) {
        return ram[(address % 0x0800)];
    }
    else if ( (address < 0x4000) && (address >= 0x2000) ) {
        switch (address & 8) {
            case 0 :
                return 0;
                break;
            case 1 :
                return 0;
                break;
            case 2 :
                ppuLatch = 0;
                uint8_t temp = (ppuStatus.reg & 0xE0) | (ppuData & 0x1F);
                ppuStatus.vBlank = 0;
                return temp;
                break;
            case 3 :
                return 0;
                break;
            case 4 :
                return 0;
                break;
            case 5 :
                return 0;
                break;
            case 6 :
                return 0;
                break;
            case 7 :
                uint8_t temp = ppuData;
                ppuData = ppu_read(loopyV.reg);
                if (address >= 0x3F00)  
                    temp = ppuData;
                loopyV.reg += ( (ppuControl.incrementMode) ? 32 : 1);
                return temp;
                break;
        }
    }
    else if ( (address < 0x4020) && (address >= 0x4000) ) {
        // APU and IO Registers
    }
    else if ( (address <= 0xFFFF) && (address >= 0x4020) ) {
        return mappers[mapper].cpuRead(address);
    }
    return 0;
}