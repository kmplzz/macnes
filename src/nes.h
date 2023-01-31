#ifndef MACNES_NES_H
#define MACNES_NES_H

#include <stdlib.h>
#include "defs.h"
#include "ram.h"
#include "bus.h"
#include "cpu.h"

NES nes_init() {
    Bus *bus = bus_init();

    RAM *ram = ram_init();
    bus_connect_ram(bus, ram);

    CPU *cpu = cpu_init();
    bus_connect_cpu(bus, cpu);

    NES nes = {ram, bus, cpu};
    return nes;
}

void nes_shutdown(NES nes) {
    free(nes.ram);
    free(nes.cpu);
    free(nes.bus);
}

#endif
