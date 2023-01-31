#include "nes.h"

int main() {
    NES nes = nes_init();
    cpu_clock(nes.cpu);
    nes_shutdown(nes);
    return 0;
}
