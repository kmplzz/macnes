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
    Bus         *bus;

    uint8_t     a;
    uint8_t     x;
    uint8_t     y;
    uint8_t     sp;
    uint8_t     status;
    uint16_t    pc;

    uint8_t     fetched;
    uint16_t    addr_abs;
    uint16_t    addr_rel;
    uint8_t     opcode;
    uint8_t     cycles;
    uint32_t    clock_count;
} CPU;

typedef struct {
    RAM *ram;
    Bus *bus;
    CPU *cpu;
} NES;

#endif
