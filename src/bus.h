#ifndef MACNES_BUS_H
#define MACNES_BUS_H

#include "defs.h"

Bus* bus_init();

void bus_connect_ram(Bus *bus, RAM *ram);

void bus_connect_cpu(Bus *bus, CPU *cpu);

uint8_t bus_read(Bus *bus, uint16_t address);

void bus_write(Bus *bus, uint16_t address, uint8_t data);

#endif
