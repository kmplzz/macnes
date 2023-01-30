#include <stdlib.h>
#include "bus.h"

Bus* bus_init() {
    Bus *bus = malloc(sizeof(Bus));
    return bus;
}

void bus_connect_ram(Bus *bus, RAM *ram) {
    bus->ram = ram;
}

void bus_connect_cpu(Bus *bus, CPU *cpu) {
    cpu->bus = bus;
}