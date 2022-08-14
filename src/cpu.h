#ifndef CPU_H
#define CPU_H

#include <stdint.h>

struct cpu_state;

struct cpu_state cpu_init();
void cpu_clock();

#endif