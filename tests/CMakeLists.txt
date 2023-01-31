cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)

find_package(GTest REQUIRED)

add_executable(tests ram_tests.cc cpu_tests.cc)

include(GoogleTest)
include_directories(../src)
target_link_libraries(tests GTest::gtest_main nes)

enable_testing()
gtest_discover_tests(tests)
