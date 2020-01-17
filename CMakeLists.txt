cmake_minimum_required(VERSION 3.0.0)
project(xorshift32_16bit_simd)
set(CMAKE_CXX_STANDARD 11)
add_executable(test test.cpp xorshift32_16bit_simd.c xorshift32_16bit_simd.h dev.hpp)