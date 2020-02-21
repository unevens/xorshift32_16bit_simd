cmake_minimum_required(VERSION 3.0.0)
project(xorshift32_16bit_simd)
set(CMAKE_CXX_STANDARD 11)
add_executable(test test.cpp xorshift32_16bit_simd.c xorshift32_16bit_simd.h dev.hpp)

if(UNIX)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse4.1")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")
endif(UNIX)