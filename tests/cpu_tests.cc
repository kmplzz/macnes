#include <gtest/gtest.h>

extern "C" {
    #include "cpu.c"
    #include "defs.h"
    #include "nes.h"
}
#define SUITE CPU



// Addressing modes

TEST(SUITE, check_am_imp) {
    CPU *cpu = cpu_init();

    uint8_t cycles = am_IMP(cpu);

    EXPECT_EQ(cpu->a, cpu->fetched);
    EXPECT_EQ(0, cycles);
    EXPECT_EQ(0, cpu->pc);

    cpu_destroy(cpu);
}

TEST(SUITE, check_am_imm) {
    CPU *cpu = cpu_init();

    uint8_t cycles = am_IMM(cpu);

    EXPECT_EQ(cpu->pc - 1, cpu->addr_abs);
    EXPECT_EQ(0, cycles);
    EXPECT_EQ(1, cpu->pc);

    cpu_destroy(cpu);
}

TEST(SUITE, check_am_zp0) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    ram_write(nes.ram, 0, 0xAA);

    uint8_t cycles = am_ZP0(cpu);

    EXPECT_EQ(0x00AA, cpu->addr_abs);
    EXPECT_EQ(0, cycles);
    EXPECT_EQ(1, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_am_zpx) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->x = 1;
    ram_write(nes.ram, 0, 0xAA);

    uint8_t cycles = am_ZPX(cpu);

    EXPECT_EQ(0x00AB, cpu->addr_abs);
    EXPECT_EQ(0, cycles);
    EXPECT_EQ(1, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_am_zpy) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->y = 2;
    ram_write(nes.ram, 0, 0xAA);

    uint8_t cycles = am_ZPY(cpu);

    EXPECT_EQ(0x00AC, cpu->addr_abs);
    EXPECT_EQ(0, cycles);
    EXPECT_EQ(1, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_am_rel) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    ram_write(nes.ram, 0, 0x1);

    uint8_t cycles = am_REL(cpu);

    EXPECT_EQ(0x1, cpu->addr_rel);
    EXPECT_EQ(0, cycles);
    EXPECT_EQ(1, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_am_rel1) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    ram_write(nes.ram, 0, 0xFA);

    uint8_t cycles = am_REL(cpu);

    EXPECT_EQ(0xFFFA, cpu->addr_rel);
    EXPECT_EQ(0, cycles);
    EXPECT_EQ(1, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_am_abs) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    ram_write(nes.ram, 0, 0xCD);
    ram_write(nes.ram, 1, 0xAB);

    uint8_t cycles = am_ABS(cpu);

    EXPECT_EQ(0xABCD, cpu->addr_abs);
    EXPECT_EQ(0, cycles);
    EXPECT_EQ(2, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_am_abx_current_page) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->x = 1;
    ram_write(nes.ram, 0, 0xCD);
    ram_write(nes.ram, 1, 0xAB);

    uint8_t cycles = am_ABX(cpu);

    EXPECT_EQ(0xABCE, cpu->addr_abs);
    EXPECT_EQ(0, cycles);
    EXPECT_EQ(2, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_am_abx_additional_page) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->x = 1;
    ram_write(nes.ram, 0, 0xFF);
    ram_write(nes.ram, 1, 0xAB);

    uint8_t cycles = am_ABX(cpu);

    EXPECT_EQ(0xAC00, cpu->addr_abs);
    EXPECT_EQ(1, cycles);
    EXPECT_EQ(2, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_am_aby_current_page) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->y = 1;
    ram_write(nes.ram, 0, 0xCD);
    ram_write(nes.ram, 1, 0xAB);

    uint8_t cycles = am_ABY(cpu);

    EXPECT_EQ(0xABCE, cpu->addr_abs);
    EXPECT_EQ(0, cycles);
    EXPECT_EQ(2, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_am_aby_next_page) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->y = 1;
    ram_write(nes.ram, 0, 0xFF);
    ram_write(nes.ram, 1, 0xAB);

    uint8_t cycles = am_ABY(cpu);

    EXPECT_EQ(0xAC00, cpu->addr_abs);
    EXPECT_EQ(1, cycles);
    EXPECT_EQ(2, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_am_ind) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    ram_write(nes.ram, 0, 0xCD);
    ram_write(nes.ram, 1, 0xAB);
    ram_write(nes.ram, 0xABCD, 0xBB);
    ram_write(nes.ram, 0xABCE, 0xAA);

    uint8_t cycles = am_IND(cpu);

    EXPECT_EQ(0xAABB, cpu->addr_abs);
    EXPECT_EQ(0, cycles);
    EXPECT_EQ(2, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_am_ind_cross_page) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    ram_write(nes.ram, 0, 0xFF);
    ram_write(nes.ram, 1, 0xAB);
    ram_write(nes.ram, 0xAB00, 0xBB);
    ram_write(nes.ram, 0xABFF, 0xCC);

    uint8_t cycles = am_IND(cpu);

    EXPECT_EQ(0xBBCC, cpu->addr_abs);
    EXPECT_EQ(0, cycles);
    EXPECT_EQ(2, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_am_izx) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->x = 1;
    ram_write(nes.ram, 0, 0xAA);
    ram_write(nes.ram, 0x00AB, 0xCD);
    ram_write(nes.ram, 0x00AC, 0xAB);

    uint8_t cycles = am_IZX(cpu);

    EXPECT_EQ(0xABCD, cpu->addr_abs);
    EXPECT_EQ(0, cycles);
    EXPECT_EQ(1, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_am_izy_current_page) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->y = 1;
    ram_write(nes.ram, 0, 0xAA);
    ram_write(nes.ram, 0x00AA, 0xCD);
    ram_write(nes.ram, 0x00AB, 0xAB);

    uint8_t cycles = am_IZY(cpu);

    EXPECT_EQ(0xABCE, cpu->addr_abs);
    EXPECT_EQ(0, cycles);
    EXPECT_EQ(1, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_am_izy_additional_page) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->y = 1;
    ram_write(nes.ram, 0, 0xAA);
    ram_write(nes.ram, 0xAA, 0xFF);
    ram_write(nes.ram, 0xAB, 0xAB);

    uint8_t cycles = am_IZY(cpu);

    EXPECT_EQ(0xAC00, cpu->addr_abs);
    EXPECT_EQ(1, cycles);
    EXPECT_EQ(1, cpu->pc);

    nes_shutdown(nes);
}
