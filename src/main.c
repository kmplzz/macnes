#include "nes.h"

int main() {
    NES nes = nes_init();
    cpu_clock(nes.cpu);
    return 0;
}
