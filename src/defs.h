#ifndef MACNES_DEFS_H
#define MACNES_DEFS_H

#include "stdint.h"
#include "stdbool.h"

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

    uint16_t    addr_abs;
    uint16_t    addr_rel;
    uint8_t     opcode;
    bool        is_am_imm;
    uint8_t     cycles;
    uint32_t    clock_count;
} CPU;

enum CpuFlag {
    C = (1 << 0),
    Z = (1 << 1),
    I = (1 << 2),
    D = (1 << 3),
    B = (1 << 4),
    U = (1 << 5),
    V = (1 << 6),
    N = (1 << 7),
};

typedef struct {
    RAM *ram;
    Bus *bus;
    CPU *cpu;
} NES;

#endif
