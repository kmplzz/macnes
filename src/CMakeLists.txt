cmake_minimum_required(VERSION 3.22)
project(Nes C)

set(CMAKE_C_STANDARD 99)

add_library(nes STATIC ram.h ram.c bus.c cpu.c defs.h nes.h)
