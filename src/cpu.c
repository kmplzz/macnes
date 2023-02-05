#include <stdlib.h>
#include <stdbool.h>
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

uint8_t cpu_fetch_operand(CPU *cpu) {
    return cpu->is_am_imm ? cpu->a : bus_read(cpu->bus, cpu->addr_abs);
}



// Flags

uint8_t cpu_get_flag(CPU *cpu, enum CpuFlag flag) {
    return (cpu->status & flag) > 0 ? 1 : 0;
}

void cpu_set_flag(CPU *cpu, enum CpuFlag flag, bool value) {
    if (value) cpu->status |= flag;
    else cpu->status &= ~flag;
}



// Addressing modes

uint8_t am_IMP(CPU *cpu) {
    cpu->is_am_imm = true;
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



// Instructions

uint8_t i_ADC(CPU *cpu) {
    uint8_t op = cpu_fetch_operand(cpu);
    uint16_t temp = (uint16_t) cpu->a + (uint16_t) op + (uint16_t) cpu_get_flag(cpu, C);
    cpu_set_flag(cpu, C, temp > 0xFF);
    cpu_set_flag(cpu, Z, (temp & 0x00FF) == 0);
    cpu_set_flag(cpu, V, (~((uint16_t) cpu->a ^ (uint16_t) op) & ((uint16_t) cpu->a ^ (uint16_t)temp)) & 0x0080);
    cpu_set_flag(cpu, N, (temp & 0x80));
    cpu->a = temp & 0x00FF;
    return 1;
}

uint8_t i_SBC(CPU *cpu) {
    uint8_t op = cpu_fetch_operand(cpu);
    uint16_t value = (uint16_t) op ^ 0x00FF;
    uint16_t temp = (uint16_t) cpu->a + value + (uint16_t) cpu_get_flag(cpu, C);
    cpu_set_flag(cpu, C, temp > 0xFF);
    cpu_set_flag(cpu, Z, (temp & 0x00FF) == 0);
    cpu_set_flag(cpu, V, (temp ^ (uint16_t) cpu->a) & (temp ^ value) & 0x0080);
    cpu_set_flag(cpu, N, temp & 0x80);
    cpu->a = temp & 0x00FF;
    return 1;
}

uint8_t i_AND(CPU *cpu) {
    uint8_t op = cpu_fetch_operand(cpu);
    cpu->a &= op;
    cpu_set_flag(cpu, Z, cpu->a == 0);
    cpu_set_flag(cpu, N, cpu->a & 0x80);
    return 1;
}

uint8_t i_ASL(CPU *cpu) {
    uint16_t op = cpu_fetch_operand(cpu);
    uint16_t value = op << 1;
    cpu_set_flag(cpu, C, value & 0xFF00);
    cpu_set_flag(cpu, Z, (value & 0x00FF) == 0);
    cpu_set_flag(cpu, N, value & 0x80);
    if (cpu->is_am_imm) cpu->a = value & 0x00FF;
    else bus_write(cpu->bus, cpu->addr_abs, value & 0x00FF);
    return 0;
}

uint8_t cpu_branch_conditional(CPU *cpu, bool condition) {
    if (!condition) return 0;
    cpu->cycles++;
    cpu->addr_abs = cpu->pc + cpu->addr_rel;
    if ((cpu->addr_abs & 0xFF00) != (cpu->pc & 0xFF00))
        cpu->cycles++;
    cpu->pc = cpu->addr_abs;
    return 0;
}

uint8_t i_BCC(CPU *cpu) {
    return cpu_branch_conditional(cpu, !cpu_get_flag(cpu, C));
}

uint8_t i_BCS(CPU *cpu) {
    return cpu_branch_conditional(cpu, cpu_get_flag(cpu, C));
}

uint8_t i_BEQ(CPU *cpu) {
    return cpu_branch_conditional(cpu, cpu_get_flag(cpu, Z));
}

uint8_t i_BIT(CPU *cpu) {
    uint8_t op = cpu_fetch_operand(cpu);
    uint16_t temp = cpu->a & op;
    cpu_set_flag(cpu, Z, (temp & 0x00FF) == 0);
    cpu_set_flag(cpu, N, op & (1 << 7));
    cpu_set_flag(cpu, V, op & (1 << 6));
    return 0;
}

uint8_t i_BMI(CPU *cpu) {
    return cpu_branch_conditional(cpu, cpu_get_flag(cpu, N));
}

uint8_t i_BNE(CPU *cpu) {
    return cpu_branch_conditional(cpu, !cpu_get_flag(cpu, Z));
}

uint8_t i_BPL(CPU *cpu) {
    return cpu_branch_conditional(cpu, !cpu_get_flag(cpu, N));
}

uint8_t i_BRK(CPU *cpu) {
    cpu->pc++;
    cpu_set_flag(cpu, I, true);
    bus_write(cpu->bus, 0x0100 + cpu->sp, (cpu->pc >> 8) & 0x00FF);
    cpu->sp--;
    bus_write(cpu->bus, 0x0100 + cpu->sp, cpu->pc & 0x00FF);
    cpu->sp--;
    cpu_set_flag(cpu, B, true);
    cpu->pc = (uint16_t) bus_read(cpu->bus, 0xFFFE)
            | ((uint16_t) bus_read(cpu->bus, 0xFFFF) << 8);
    return 0;
}

uint8_t i_BVC(CPU *cpu) {
    return cpu_branch_conditional(cpu, !cpu_get_flag(cpu, V));
}

uint8_t i_BVS(CPU *cpu) {
    return cpu_branch_conditional(cpu, cpu_get_flag(cpu, V));
}

uint8_t i_CLC(CPU *cpu) {
    cpu_set_flag(cpu, C, false);
    return 0;
}

uint8_t i_CLD(CPU *cpu) {
    cpu_set_flag(cpu, D, false);
    return 0;
}

uint8_t i_CLI(CPU *cpu) {
    cpu_set_flag(cpu, I, false);
    return 0;
}

uint8_t i_CLV(CPU *cpu) {
    cpu_set_flag(cpu, V, false);
    return 0;
}

uint8_t i_CMP(CPU *cpu) {
    uint8_t  op = cpu_fetch_operand(cpu);
    uint16_t temp =  (uint16_t) cpu->a - (uint16_t) op;
    cpu_set_flag(cpu, C, cpu->a >= op);
    cpu_set_flag(cpu, Z, (temp & 0x00FF) == 0);
    cpu_set_flag(cpu, N, temp & 0x0080);
    return 1;
}

uint8_t i_CPX(CPU *cpu) {
    uint8_t op = cpu_fetch_operand(cpu);
    uint16_t temp =  (uint16_t) cpu->x - (uint16_t) op;
    cpu_set_flag(cpu, C, cpu->x >= op);
    cpu_set_flag(cpu, Z, (temp & 0x00FF) == 0);
    cpu_set_flag(cpu, N, temp & 0x0080);
    return 0;
}

uint8_t i_CPY(CPU *cpu) {
    uint8_t op = cpu_fetch_operand(cpu);
    uint16_t temp = (uint16_t) cpu->y - (uint16_t) op;
    cpu_set_flag(cpu, C, cpu->y >= op);
    cpu_set_flag(cpu, Z, (temp & 0x00FF) == 0);
    cpu_set_flag(cpu, N, temp & 0x0080);
    return 0;
}

uint8_t i_DEC(CPU *cpu) {
    uint8_t op = cpu_fetch_operand(cpu);
    uint16_t temp = op - 1;
    bus_write(cpu->bus, cpu->addr_abs, temp & 0x00FF);
    cpu_set_flag(cpu, Z, (temp & 0x00FF) == 0);
    cpu_set_flag(cpu, N, temp & 0x0080);
    return 0;
}

uint8_t i_DEX(CPU *cpu) {
    cpu->x--;
    cpu_set_flag(cpu, Z, cpu->x == 0x00);
    cpu_set_flag(cpu, N, cpu->x & 0x80);
    return 0;
}

uint8_t i_DEY(CPU *cpu) {
    cpu->y--;
    cpu_set_flag(cpu, Z, cpu->y == 0x00);
    cpu_set_flag(cpu, N, cpu->y & 0x80);
    return 0;
}

uint8_t i_EOR(CPU *cpu) {
    uint8_t op = cpu_fetch_operand(cpu);
    cpu->a ^= op;
    cpu_set_flag(cpu, Z, cpu->a == 0x00);
    cpu_set_flag(cpu, N, cpu->a & 0x80);
    return 1;
}

uint8_t i_INC(CPU *cpu) {
    uint8_t op = cpu_fetch_operand(cpu);
    uint16_t temp = op + 1;
    bus_write(cpu->bus, cpu->addr_abs, temp & 0x00FF);
    cpu_set_flag(cpu, Z, (temp & 0x00FF) == 0x0000);
    cpu_set_flag(cpu, N, temp & 0x0080);
    return 0;
}

uint8_t i_INX(CPU *cpu) {
    cpu->x++;
    cpu_set_flag(cpu, Z, cpu->x == 0);
    cpu_set_flag(cpu, N, cpu->x & 0x80);
    return 0;
}

uint8_t i_INY(CPU *cpu) {
    cpu->y++;
    cpu_set_flag(cpu, Z, cpu->y == 0);
    cpu_set_flag(cpu, N, cpu->y & 0x80);
    return 0;
}

uint8_t i_JMP(CPU *cpu) {
    cpu->pc = cpu->addr_abs;
    return 0;
}

uint8_t i_JSR(CPU *cpu) {
    cpu->pc--;
    bus_write(cpu->bus, 0x0100 + cpu->sp, (cpu->pc >> 8) & 0x00FF);
    cpu->sp--;
    bus_write(cpu->bus, 0x0100 + cpu->sp, cpu->pc & 0x00FF);
    cpu->sp--;
    cpu->pc = cpu->addr_abs;
    return 0;
}

uint8_t i_LDA(CPU *cpu) {
    cpu->a = cpu_fetch_operand(cpu);
    cpu_set_flag(cpu, Z, cpu->a == 0x00);
    cpu_set_flag(cpu, N, cpu->a & 0x80);
    return 0;
}

uint8_t i_LDX(CPU *cpu) {
    cpu->x = cpu_fetch_operand(cpu);
    cpu_set_flag(cpu, Z, cpu->x == 0x00);
    cpu_set_flag(cpu, N, cpu->x & 0x80);
    return 0;
}

uint8_t i_LDY(CPU *cpu) {
    cpu->y = cpu_fetch_operand(cpu);
    cpu_set_flag(cpu, Z, cpu->y == 0x00);
    cpu_set_flag(cpu, N, cpu->y & 0x80);
    return 0;
}

uint8_t i_LSR(CPU *cpu) {
    // TODO
    return 0;
}

uint8_t i_NOP(CPU *cpu) {
    switch (cpu->opcode) {
        case 0x1C:
        case 0x3C:
        case 0x5C:
        case 0x7C:
        case 0xDC:
        case 0xFC:
            return 1;
    }
    return 0;
}

uint8_t i_ORA(CPU *cpu) {
    cpu->a |= cpu_fetch_operand(cpu);
    cpu_set_flag(cpu, Z, cpu->a == 0x00);
    cpu_set_flag(cpu, N, cpu->a & 0x80);
    return 0;
}

uint8_t i_PHA(CPU *cpu) {
    bus_write(cpu->bus, 0x0100 + cpu->sp, cpu->a);
    cpu->sp--;
    return 0;
}

uint8_t i_PHP(CPU *cpu) {
    cpu_set_flag(cpu, B | U, true);
    bus_write(cpu->bus, 0x0100 + cpu->sp, cpu->status);
    cpu->sp--;
    return 0;
}

uint8_t i_PLA(CPU *cpu) {
    cpu->sp++;
    cpu->a = bus_read(cpu->bus, 0x0100 + cpu->sp);
    cpu_set_flag(cpu, Z, cpu->a == 0x00);
    cpu_set_flag(cpu, N, cpu->a * 0x80);
    return 0;
}

uint8_t i_PLP(CPU *cpu) {
    cpu->sp++;
    cpu->status = bus_read(cpu->bus, 010100 + cpu->sp);
    cpu_set_flag(cpu, U, true);
    return 0;
}

uint8_t i_ROL(CPU *cpu) {
    // TODO
    return 0;
}

uint8_t i_ROR(CPU *cpu) {
    // TODO
    return 0;
}

uint8_t i_RTI(CPU *cpu) {
    // TODO
    return 0;
}

uint8_t i_RTS(CPU *cpu) {
    cpu->sp++;
    cpu->pc = bus_read(cpu->bus, 0x0100 + cpu->sp);
    cpu->sp++;
    cpu->pc |= bus_read(cpu->bus, 0x0100 + cpu->sp) << 8;
    cpu->pc++;
    return 0;
}

uint8_t i_SEC(CPU *cpu) {
    cpu_set_flag(cpu, C, true);
    return 0;
}

uint8_t i_SED(CPU *cpu) {
    cpu_set_flag(cpu, D, true);
    return 0;
}

uint8_t i_SEI(CPU *cpu) {
    cpu_set_flag(cpu, I, true);
    return 0;
}

uint8_t i_STA(CPU *cpu) {
    bus_write(cpu->bus, cpu->addr_abs, cpu->a);
    return 0;
}

uint8_t i_STX(CPU *cpu) {
    bus_write(cpu->bus, cpu->addr_abs, cpu->x);
    return 0;
}

uint8_t i_STY(CPU *cpu) {
    bus_write(cpu->bus, cpu->addr_abs, cpu->y);
    return 0;
}

uint8_t i_TAX(CPU *cpu) {
    cpu->x = cpu->a;
    cpu_set_flag(cpu, Z, cpu->x == 0x00);
    cpu_set_flag(cpu, N, cpu->x & 0x80);
    return 0;
}

uint8_t i_TAY(CPU *cpu) {
    cpu->y = cpu->a;
    cpu_set_flag(cpu, Z, cpu->y == 0x00);
    cpu_set_flag(cpu, N, cpu->y & 0x80);
    return 0;
}

uint8_t i_TSX(CPU *cpu) {
    cpu->x = cpu->sp;
    cpu_set_flag(cpu, Z, cpu->x == 0x00);
    cpu_set_flag(cpu, N, cpu->x & 0x80);
    return 0;
}

uint8_t i_TXA(CPU *cpu) {
    cpu->a = cpu->x;
    cpu_set_flag(cpu, Z, cpu->a == 0x00);
    cpu_set_flag(cpu, N, cpu->a & 0x80);
    return 0;
}

uint8_t i_TXS(CPU *cpu) {
    cpu->sp = cpu->x;
    return 0;
}

uint8_t i_TYA(CPU *cpu) {
    cpu->a = cpu->y;
    cpu_set_flag(cpu, Z, cpu->a == 0x00);
    cpu_set_flag(cpu, N, cpu->a & 0x80);
    return 0;
}

uint8_t i_XXX(CPU *cpu) {
    return 0;
}
