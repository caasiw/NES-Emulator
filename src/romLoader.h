#ifndef ROMLOADER_H
#define ROMLOADER_H

#include <stdio.h>

void parseRom(FILE *rom);
FILE* selectRom();

#endif