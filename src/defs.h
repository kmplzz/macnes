#ifndef MACNES_DEFS_H
#define MACNES_DEFS_H

#include "stdint.h"

#define RAM_SIZE 64 * 1024

typedef struct {
    uint8_t data[RAM_SIZE];
} RAM;

typedef struct {
    RAM *ram;
} Bus;

typedef struct {
    Bus *bus;
} CPU;

typedef struct {
    RAM *ram;
    Bus *bus;
    CPU *cpu;
} NES;

#endif
