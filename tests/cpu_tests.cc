#include <gtest/gtest.h>

extern "C" {
    #include "cpu.c"
    #include "defs.h"
    #include "nes.h"
}
#define SUITE CPU

// Flags

TEST(SUITE, check_flags) {
    CPU *cpu = cpu_init();
    cpu_set_flag(cpu, D, true);
    cpu_set_flag(cpu, B, true);
    cpu_set_flag(cpu, U, false);

    uint8_t d_val = cpu_get_flag(cpu, D);
    uint8_t b_val = cpu_get_flag(cpu, B);
    uint8_t u_val = cpu_get_flag(cpu, U);

    EXPECT_TRUE(d_val);
    EXPECT_TRUE(b_val);
    EXPECT_FALSE(u_val);

    cpu_destroy(cpu);
}



// Addressing modes

TEST(SUITE, check_am_imp) {
    CPU *cpu = cpu_init();

    uint8_t cycles = am_IMP(cpu);

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

TEST(SUITE, check_am_rel_negative) {
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



// Instructions

TEST(SUITE, check_i_adc) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->a = 10;
    ram_write(nes.ram, 0, 22);
    uint8_t expected_flags = 0;

    uint8_t cycles = i_ADC(cpu);

    EXPECT_EQ(32, cpu->a);
    EXPECT_EQ(1, cycles);
    EXPECT_EQ(expected_flags, cpu->status);

    nes_shutdown(nes);
}

TEST(SUITE, check_i_adc_carry) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->a = 10;
    ram_write(nes.ram, 0, 250);
    uint8_t expected_flags = C;

    uint8_t cycles = i_ADC(cpu);

    EXPECT_EQ(4, cpu->a);
    EXPECT_EQ(1, cycles);
    EXPECT_EQ(expected_flags, cpu->status);

    nes_shutdown(nes);
}

TEST(SUITE, check_i_adc_zero) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->a = 0;
    ram_write(nes.ram, 0, 0);
    uint8_t expected_flags = Z;

    uint8_t cycles = i_ADC(cpu);

    EXPECT_EQ(0, cpu->a);
    EXPECT_EQ(1, cycles);
    EXPECT_EQ(expected_flags, cpu->status);

    nes_shutdown(nes);
}

TEST(SUITE, check_i_adc_overflow) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->a = 250;
    ram_write(nes.ram, 0, 1);
    uint8_t expected_flags = V;

    uint8_t cycles = i_ADC(cpu);

    // TODO

    nes_shutdown(nes);
}
TEST(SUITE, check_i_sbc) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->a = 255;
    ram_write(nes.ram, 0, 254);
    uint8_t expected_flags = 0;

    uint8_t cycles = i_SBC(cpu);

    // TODO

    nes_shutdown(nes);
}

TEST(SUITE, check_i_sbc_carry) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->a = 10;
    ram_write(nes.ram, 0, 250);
    uint8_t expected_flags = C;

    uint8_t cycles = i_ADC(cpu);

    // TODO

    nes_shutdown(nes);
}

TEST(SUITE, check_i_sbc_zero) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->a = 0;
    ram_write(nes.ram, 0, 0);
    uint8_t expected_flags = Z;

    uint8_t cycles = i_ADC(cpu);

    // TODO

    nes_shutdown(nes);
}

TEST(SUITE, check_i_sbc_overflow) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->a = 250;
    ram_write(nes.ram, 0, 1);
    uint8_t expected_flags = V;

    uint8_t cycles = i_ADC(cpu);

    // TODO

    nes_shutdown(nes);
}


TEST(SUITE, check_i_adc_negative) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->a = 250;
    ram_write(nes.ram, 0, 1);
    uint8_t expected_flags = N;

    uint8_t cycles = i_ADC(cpu);

    // TODO

    nes_shutdown(nes);
}

TEST(SUITE, check_i_and) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->a = 3;
    ram_write(nes.ram, 0, 1);
    uint8_t expected_flags = 0;

    uint8_t cycles = i_AND(cpu);

    EXPECT_EQ(1, cycles);
    EXPECT_EQ(1, cpu->a);
    EXPECT_EQ(expected_flags, cpu->status);

    nes_shutdown(nes);
}

TEST(SUITE, check_i_and_zero) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->a = 2;
    ram_write(nes.ram, 0, 1);
    uint8_t expected_flags = Z;

    uint8_t cycles = i_AND(cpu);

    EXPECT_EQ(1, cycles);
    EXPECT_EQ(0, cpu->a);
    EXPECT_EQ(expected_flags, cpu->status);

    nes_shutdown(nes);
}

TEST(SUITE, check_i_and_negative) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->a = 128;
    ram_write(nes.ram, 0, 128);
    uint8_t expected_flags = N;

    uint8_t cycles = i_AND(cpu);

    EXPECT_EQ(1, cycles);
    EXPECT_EQ(128, cpu->a);
    EXPECT_EQ(expected_flags, cpu->status);

    nes_shutdown(nes);
}

TEST(SUITE, check_i_asl) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->is_am_imm = true;
    cpu->a = 1;
    uint8_t expected_flags = 0;

    uint8_t cycles = i_ASL(cpu);

    EXPECT_EQ(0, cycles);
    EXPECT_EQ(2, cpu->a);
    EXPECT_EQ(expected_flags, cpu->status);

    nes_shutdown(nes);
}

TEST(SUITE, check_i_asl_carry) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->is_am_imm = true;
    cpu->a = 0x81;
    uint8_t expected_flags = C;

    uint8_t cycles = i_ASL(cpu);

    EXPECT_EQ(0, cycles);
    EXPECT_EQ(2, cpu->a);
    EXPECT_EQ(expected_flags, cpu->status);

    nes_shutdown(nes);
}

TEST(SUITE, check_i_asl_zero) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->is_am_imm = true;
    cpu->a = 0;
    uint8_t expected_flags = Z;

    uint8_t cycles = i_ASL(cpu);

    EXPECT_EQ(0, cycles);
    EXPECT_EQ(0, cpu->a);
    EXPECT_EQ(expected_flags, cpu->status);

    nes_shutdown(nes);
}

TEST(SUITE, check_i_asl_negative) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->is_am_imm = true;
    cpu->a = 0x40;
    uint8_t expected_flags = N;

    uint8_t cycles = i_ASL(cpu);

    EXPECT_EQ(0, cycles);
    EXPECT_EQ(0x80, cpu->a);
    EXPECT_EQ(expected_flags, cpu->status);

    nes_shutdown(nes);
}

TEST(SUITE, check_cpu_branch_conditional_cond_true_current_page) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->cycles = 0;
    cpu->pc = 0x0002;
    cpu->addr_rel = 0x0001;

    uint8_t cycles = cpu_branch_conditional(cpu, true);

    EXPECT_EQ(0, cycles);
    EXPECT_EQ(1, cpu->cycles);
    EXPECT_EQ(0x0003, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_cpu_branch_conditional_cond_true_next_page) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->cycles = 0;
    cpu->pc = 0x00FF;
    cpu->addr_rel = 0x0002;

    uint8_t cycles = cpu_branch_conditional(cpu, true);

    EXPECT_EQ(0, cycles);
    EXPECT_EQ(2, cpu->cycles);
    EXPECT_EQ(0x0101, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_cpu_branch_conditional_cond_false) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->cycles = 0;
    cpu->pc = 0x00FF;
    cpu->addr_rel = 0x0002;

    uint8_t cycles = cpu_branch_conditional(cpu, false);

    EXPECT_EQ(0, cycles);
    EXPECT_EQ(0, cpu->cycles);
    EXPECT_EQ(0x00FF, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_i_bcc_true) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->pc = 0x0001;
    cpu->addr_rel = 0x0002;

    uint8_t cycles = i_BCC(cpu);

    EXPECT_EQ(0, cycles);
    EXPECT_EQ(0x0003, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_i_bcc_false) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->status = C;
    cpu->pc = 0x0001;
    cpu->addr_rel = 0x0002;

    uint8_t cycles = i_BCC(cpu);

    EXPECT_EQ(0, cycles);
    EXPECT_EQ(0x0001, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_i_bcs_true) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->status = C;
    cpu->pc = 0x0001;
    cpu->addr_rel = 0x0002;

    uint8_t cycles = i_BCS(cpu);

    EXPECT_EQ(0, cycles);
    EXPECT_EQ(0x0003, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_i_bcs_false) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->pc = 0x0001;
    cpu->addr_rel = 0x0002;

    uint8_t cycles = i_BCS(cpu);

    EXPECT_EQ(0, cycles);
    EXPECT_EQ(0x0001, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_i_beq_true) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->status = Z;
    cpu->pc = 0x0001;
    cpu->addr_rel = 0x0002;

    uint8_t cycles = i_BEQ(cpu);

    EXPECT_EQ(0, cycles);
    EXPECT_EQ(0x0003, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_i_beq_false) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->pc = 0x0001;
    cpu->addr_rel = 0x0002;

    uint8_t cycles = i_BEQ(cpu);

    EXPECT_EQ(0, cycles);
    EXPECT_EQ(0x0001, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_i_bit) {
    // TODO
}

TEST(SUITE, check_i_bmi_true) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->status = N;
    cpu->pc = 0x0001;
    cpu->addr_rel = 0x0002;

    uint8_t cycles = i_BMI(cpu);

    EXPECT_EQ(0, cycles);
    EXPECT_EQ(0x0003, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_i_bmi_false) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->pc = 0x0001;
    cpu->addr_rel = 0x0002;

    uint8_t cycles = i_BMI(cpu);

    EXPECT_EQ(0, cycles);
    EXPECT_EQ(0x0001, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_i_bne_true) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->pc = 0x0001;
    cpu->addr_rel = 0x0002;

    uint8_t cycles = i_BNE(cpu);

    EXPECT_EQ(0, cycles);
    EXPECT_EQ(0x0003, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_i_bne_false) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->status = Z;
    cpu->pc = 0x0001;
    cpu->addr_rel = 0x0002;

    uint8_t cycles = i_BNE(cpu);

    EXPECT_EQ(0, cycles);
    EXPECT_EQ(0x0001, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_i_bpl_true) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->pc = 0x0001;
    cpu->addr_rel = 0x0002;

    uint8_t cycles = i_BPL(cpu);

    EXPECT_EQ(0, cycles);
    EXPECT_EQ(0x0003, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_i_bpl_false) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->status = N;
    cpu->pc = 0x0001;
    cpu->addr_rel = 0x0002;

    uint8_t cycles = i_BPL(cpu);

    EXPECT_EQ(0, cycles);
    EXPECT_EQ(0x0001, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_i_brk) {
    // TODO
}

TEST(SUITE, check_i_bvc_true) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->pc = 0x0001;
    cpu->addr_rel = 0x0002;

    uint8_t cycles = i_BVC(cpu);

    EXPECT_EQ(0, cycles);
    EXPECT_EQ(0x0003, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_i_bvc_false) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->status = V;
    cpu->pc = 0x0001;
    cpu->addr_rel = 0x0002;

    uint8_t cycles = i_BVC(cpu);

    EXPECT_EQ(0, cycles);
    EXPECT_EQ(0x0001, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_i_bvs_true) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->status = V;
    cpu->pc = 0x0001;
    cpu->addr_rel = 0x0002;

    uint8_t cycles = i_BVS(cpu);

    EXPECT_EQ(0, cycles);
    EXPECT_EQ(0x0003, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_i_bvs_false) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->pc = 0x0001;
    cpu->addr_rel = 0x0002;

    uint8_t cycles = i_BVS(cpu);

    EXPECT_EQ(0, cycles);
    EXPECT_EQ(0x0001, cpu->pc);

    nes_shutdown(nes);
}

TEST(SUITE, check_i_clc) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->status = C;

    uint8_t cycles = i_CLC(cpu);

    EXPECT_EQ(0, cycles);
    EXPECT_EQ(0, cpu->status);

    nes_shutdown(nes);
}

TEST(SUITE, check_i_cld) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->status = D;

    uint8_t cycles = i_CLD(cpu);

    EXPECT_EQ(0, cycles);
    EXPECT_EQ(0, cpu->status);

    nes_shutdown(nes);
}

TEST(SUITE, check_i_cli) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->status = I;

    uint8_t cycles = i_CLI(cpu);

    EXPECT_EQ(0, cycles);
    EXPECT_EQ(0, cpu->status);

    nes_shutdown(nes);
}

TEST(SUITE, check_i_clv) {
    NES nes = nes_init();
    CPU *cpu = nes.cpu;
    cpu->status = V;

    uint8_t cycles = i_CLV(cpu);

    EXPECT_EQ(0, cycles);
    EXPECT_EQ(0, cpu->status);

    nes_shutdown(nes);
}

