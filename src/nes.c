#include <stdio.h>
#include "cpu.h"
#include "ppu.h"
#include "romLoader.h"
#define SDL_MAIN_HANDLED
#include "gui.h"

int main(int argc, char* args[]) {
    parseRom(selectRom());
    struct cpu_state cpu = cpu_init();
    struct ppu_state ppu = ppu_init();
    gui_init();

    while(1) {
        cpu_clock(&cpu);
        ppu_clock(&ppu);
        ppu_clock(&ppu);
        ppu_clock(&ppu);
        if (ppu.scanline == 261 && ppu.cycle == 2) {
            gui_update(ppu.pixels);
        }
    }
}