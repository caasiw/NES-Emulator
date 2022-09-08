#ifndef CPU_H
#define CPU_H

#include <stdint.h>

struct cpu_state {
    uint16_t pc, opAddress;
    uint8_t acc, x, y, sp, status;
    uint8_t opcode, byte2, byte3;
    uint8_t operand;
    int pendingNMI;
    int cycles;
};

struct cpu_state cpu_init();
void cpu_clock();

#endif