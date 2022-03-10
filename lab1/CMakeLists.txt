cmake_minimum_required(VERSION 3.21)
project(lab1 C)

set(CMAKE_C_STANDARD 99)

add_library(lab1 library.c)

add_executable(main main.c)
