#include <stdint.h>

uint8_t ram[0x0800];

void cpu_write(uint16_t address, uint8_t data) {
    if ( (address < 0x2000) && (address >= 0x0000) ) {
        ram[(address % 0x0800)] = data;
        return;
    }
    else if ( (address < 0x4000) && (address >= 0x2000) ) {
        // PPU Registers
    }
    else if ( (address < 0x4020) && (address >= 0x4000) ) {
        // APU and IO Registers
    }
    else if ( (address <= 0xFFFF) && (address >= 0x4020) ) {
        // Cartridge
    }
}

uint8_t cpu_read(uint16_t address) {
    if ( (address < 0x2000) && (address >= 0x0000) ) {
        return ram[(address % 0x0800)];
    }
    else if ( (address < 0x4000) && (address >= 0x2000) ) {
        // PPU Registers
    }
    else if ( (address < 0x4020) && (address >= 0x4000) ) {
        // APU and IO Registers
    }
    else if ( (address <= 0xFFFF) && (address >= 0x4020) ) {
        // Cartridge
    }
    return 0;
}