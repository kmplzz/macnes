#ifndef MACNES_CPU_H
#define MACNES_CPU_H

#include "defs.h"

CPU* cpu_init();

void cpu_destroy(CPU *cpu);

void cpu_clock(CPU *cpu);

uint8_t cpu_get_flag(CPU *cpu, enum CpuFlag flag);

void cpu_set_flag(CPU *cpu, enum CpuFlag flag, bool value);

#endif
