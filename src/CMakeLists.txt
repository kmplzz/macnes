cmake_minimum_required(VERSION 3.22)
project(Nes C)

set(CMAKE_C_STANDARD 99)

add_library(nes STATIC ram.h ram.c bus.c cpu.c defs.h nes.h)
target_sources(nes PRIVATE cpu.c)

add_executable(macnes main.c ram.h ram.c cpu.h cpu.c bus.h bus.c defs.h nes.h)
