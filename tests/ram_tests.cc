#include <gtest/gtest.h>

extern "C" {
    #include "ram.h"
    #include "defs.h"
}
#define SUITE RAM

TEST(SUITE, check_ram_write) {
    uint16_t address = 100;
    uint8_t expected_data = 11;

    RAM *ram = ram_init();
    ram_write(ram, address, expected_data);

    EXPECT_EQ(expected_data, ram->data[address]);
}

TEST(SUITE, check_ram_read) {
    uint16_t address = 100;
    uint8_t expected_data = 11;

    RAM *ram = ram_init();
    ram->data[address] = expected_data;

    EXPECT_EQ(expected_data, ram_read(ram, address));
}