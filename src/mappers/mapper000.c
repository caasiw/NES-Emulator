#include <stdint.h>

uint8_t *prg;
uint8_t *chr;
int prgBanks;
int chrBanks;

void m000_init(uint8_t *p, uint8_t *c, int pBanks, int cBanks) {
    prgBanks = pBanks;
    chrBanks = cBanks;
    prg = p;
    chr = c;
}

void m000_cpuWrite(uint16_t address, uint8_t data) {
}

uint8_t m000_cpuRead(uint16_t address) {
    return prg[ ((address - 0x8000) & ( (prgBanks == 1) ? 0x3FFF : 0x7FFF)) ];
}

void m000_ppuWrite(uint16_t address, uint8_t data) {
}

uint8_t m000_ppuRead(uint16_t address) {
    return chr[ (address & 0x1FFF) ];
}