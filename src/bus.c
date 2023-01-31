#include <stdlib.h>
#include "bus.h"
#include "ram.h"

Bus* bus_init() {
    return (Bus*) calloc(1, sizeof(Bus));
}

void bus_destroy(Bus *bus) {
    free(bus);
}

void bus_connect_ram(Bus *bus, RAM *ram) {
    bus->ram = ram;
}

void bus_connect_cpu(Bus *bus, CPU *cpu) {
    cpu->bus = bus;
}

uint8_t bus_read(Bus *bus, uint16_t address) {
    return ram_read(bus->ram, address);
}

void bus_write(Bus *bus, uint16_t address, uint8_t data) {
    ram_write(bus->ram, address, data);
}