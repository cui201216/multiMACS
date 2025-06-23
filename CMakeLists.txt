cmake_minimum_required(VERSION 3.30)
project(makeMulti)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(makeMulti main.cpp)
