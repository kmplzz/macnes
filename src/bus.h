#ifndef MACNES_BUS_H
#define MACNES_BUS_H

#include "defs.h"

Bus* bus_init();

void bus_connect_ram(Bus *bus, RAM *ram);

void bus_connect_cpu(Bus *bus, CPU *cpu);

#endif
