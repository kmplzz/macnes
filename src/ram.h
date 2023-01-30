#ifndef MACNES_RAM_H
#define MACNES_RAM_H

#include "stdint.h"
#include "defs.h"

RAM* ram_init();

void ram_write(RAM *ram, uint16_t address, uint8_t data);

uint8_t ram_read(RAM *ram, uint16_t address);

#endif
