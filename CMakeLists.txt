cmake_minimum_required(VERSION 3.29)
project(AudioVisualiser C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(AudioVisualiser
    main.c
    main.h)
