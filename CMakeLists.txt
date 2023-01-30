cmake_minimum_required(VERSION 3.22)
project(macnes C)

set(CMAKE_C_STANDARD 99)

add_executable(macnes src/main.c src/ram.c src/bus.c src/cpu.c src/defs.h src/nes.h)
