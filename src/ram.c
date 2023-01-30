#include <stdlib.h>
#include "ram.h"

RAM* ram_init() {
    RAM *ram = calloc(1, sizeof(RAM));
    return ram;
}

void ram_write(RAM *ram, uint16_t address, uint8_t data) {
    ram->data[address] = data;
}

uint8_t ram_read(RAM *ram, uint16_t address) {
    return ram->data[address];
}