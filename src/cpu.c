#include <stdlib.h>
#include "cpu.h"
#include "bus.h"

CPU* cpu_init() {
    return (CPU*) calloc(1, sizeof(CPU));
}

void cpu_destroy(CPU *cpu) {
    free(cpu);
}

void cpu_clock(CPU *cpu) {

}



// Addressing modes

uint8_t am_IMP(CPU *cpu) {
    cpu->fetched = cpu->a;
    return 0;
}

uint8_t am_IMM(CPU *cpu) {
    cpu->addr_abs = cpu->pc++;
    return 0;
}

uint8_t am_ZP0(CPU *cpu) {
    cpu->addr_abs = bus_read(cpu->bus, cpu->pc++);
    cpu->addr_abs &= 0x00FF;
    return 0;
}

uint8_t am_ZPX(CPU *cpu) {
    cpu->addr_abs = cpu->x + bus_read(cpu->bus, cpu->pc++);
    cpu->addr_abs &= 0x00FF;
    return 0;
}

uint8_t am_ZPY(CPU *cpu) {
    cpu->addr_abs = cpu->y + bus_read(cpu->bus, cpu->pc++);
    cpu->addr_abs &= 0x00FF;
    return 0;
}

uint8_t am_REL(CPU *cpu) {
    cpu->addr_rel = bus_read(cpu->bus, cpu->pc++);
    if (cpu->addr_rel & 0x80) cpu->addr_rel |= 0xFF00;
    return 0;
}

uint8_t am_ABS(CPU *cpu) {
    uint8_t lo = bus_read(cpu->bus, cpu->pc++);
    uint8_t hi = bus_read(cpu->bus, cpu->pc++);
    cpu->addr_abs = (hi << 8) | lo;
    return 0;
}

uint8_t am_ABX(CPU *cpu) {
    uint8_t lo = bus_read(cpu->bus, cpu->pc++);
    uint8_t hi = bus_read(cpu->bus, cpu->pc++);
    cpu->addr_abs = ((hi << 8) | lo) + cpu->x;
    return (cpu->addr_abs & 0xFF00) != (hi << 8) ? 1 : 0;
}

uint8_t am_ABY(CPU *cpu) {
    uint8_t lo = bus_read(cpu->bus, cpu->pc++);
    uint8_t hi = bus_read(cpu->bus, cpu->pc++);
    cpu->addr_abs = ((hi << 8) | lo) + cpu->y;
    return (cpu->addr_abs & 0xFF00) != (hi << 8) ? 1 : 0;
}

uint8_t am_IND(CPU *cpu) {
    uint16_t ptr_lo = bus_read(cpu->bus, cpu->pc++);
    uint16_t ptr_hi = bus_read(cpu->bus, cpu->pc++);
    uint16_t ptr = (ptr_hi << 8) | ptr_lo;
    if (ptr_lo == 0x00FF)
        cpu->addr_abs = (bus_read(cpu->bus, ptr & 0xFF00) << 8)
                        | bus_read(cpu->bus, ptr);
    else
        cpu->addr_abs = (bus_read(cpu->bus, ptr + 1) << 8)
                        | bus_read(cpu->bus, ptr);
    return 0;
}

uint8_t am_IZX(CPU *cpu) {
    uint16_t t = bus_read(cpu->bus, cpu->pc++);
    uint16_t lo = bus_read(cpu->bus, (uint16_t)(t + (uint16_t) cpu->x) & 0x00FF);
    uint16_t hi = bus_read(cpu->bus, (uint16_t)(t + (uint16_t) cpu->x + 1) & 0x00FF);
    cpu->addr_abs = (hi << 8) | lo;
    return 0;
}

uint8_t am_IZY(CPU *cpu) {
    uint16_t t = bus_read(cpu->bus, cpu->pc++);
    uint16_t lo = bus_read(cpu->bus, t & 0x00FF);
    uint16_t hi = bus_read(cpu->bus, (t + 1) & 0x00FF);
    cpu->addr_abs = cpu->y + ((hi << 8) | lo);
    return (cpu->addr_abs & 0xFF00) != (hi << 8) ? 1 : 0;
}
