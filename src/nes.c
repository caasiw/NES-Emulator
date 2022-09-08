#include "cpu.h"
#include "memory.h"
#include "input.h"
#include "ppu.h"
#include "romLoader.h"
#define SDL_MAIN_HANDLED
#include "gui.h"

int main(int argc, char* args[]) {
    parseRom(selectRom());
    struct cpu_state cpu = cpu_init();
    struct ppu_state ppu = ppu_init();
    gui_init();

    int running = 1;
    while(running) {
        cpu_clock(&cpu);
        cpu.pendingNMI += ppu_clock(&ppu);
        cpu.pendingNMI += ppu_clock(&ppu);
        cpu.pendingNMI += ppu_clock(&ppu);
        if (ppu.scanline == 261 && ((ppu.cycle >= 2) && (ppu.cycle <= 4))) {
            gui_update(ppu.pixels);
            controller1 = pollController1();
            controller2 = pollController2();
            running = pollEvents();
        }
    }
    gui_stop();
}